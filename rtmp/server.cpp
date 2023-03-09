#include "server.h"
#include "base/log.hpp"
#include "connection.h"
#include <bits/types/time_t.h>
#include <cassert>
#include <cstring>
#include <strings.h>

using namespace reactor;
using namespace std::placeholders;

namespace rtmp
{

Server::Status Server::handleC01Connection(reactor::TcpConnectionPtr conn, reactor::Buffer &buffer)
{
    if (buffer.readable_bytes() != sizeof(C0) + sizeof(C1))
        return Status::ConnectWaitForC01;

    const char *data = buffer.readable_data();
    const C0 *c0 = (const C0 *)data;
    if (c0->version != 3)
        return Status::ConnectError;

    data += sizeof(C0);
    const C1 *c1 = (const C1 *)data;
    if (c1->zero != 0)
        return Status::ConnectError;
    memcpy(&connC1Cache[conn->peer_addr()], c1, sizeof(C1));

    S0 s0 = {3};
    connS1Cache[conn->peer_addr()] = {};
    S1 &s1 = connS1Cache[conn->peer_addr()];
    bzero(&s1, sizeof(S1));
    s1.epochTime = time(nullptr);
    buffer.retrive(sizeof(C0) + sizeof(C1));
    conn->send((const char *)&s0, sizeof(s0));
    conn->send((const char *)&s1, sizeof(s1));
    return Status::ConnectWaitForC2;
}

Server::Status Server::handleC2Connection(reactor::TcpConnectionPtr conn, reactor::Buffer &buffer)
{
    if (buffer.readable_bytes() != sizeof(C2))
        return Status::ConnectWaitForC2;

    const char *data = buffer.readable_data();
    const C2 *c2 = (const C2 *)data;

    S1 &s1 = connS1Cache[conn->peer_addr()];
    if (s1.epochTime != c2->epochTime || memcmp(s1.randomBytes, c2->randomEcho, sizeof(c2->randomEcho)) != 0)
    {
        return Status::ConnectError;
    }

    S2 s2;
    C1 &c1 = connC1Cache[conn->peer_addr()];
    memcpy(&s2, &c1, sizeof(s2));
    s2.epochTime = time(nullptr);
    conn->send((const char *)&s2, sizeof(S2));
    return Status::ConnectionEstablished;
}

void Server::onMessage(reactor::TcpConnectionPtr conn, reactor::Buffer &buffer, reactor::MicroTimeStamp receive_timestamp)
{
    std::string name = conn->peer_addr();
    assert(!name.empty());
    if (name.empty())
        return;

    if (connStatus_.find(name) == connStatus_.end())
        connStatus_[name] = Status::ConnectWaitForC01;

    switch (connStatus_[name])
    {
    case Status::ConnectWaitForC01:
        connStatus_[name] = handleC01Connection(conn, buffer);
        break;
    case Status::ConnectWaitForC2:
        connStatus_[name] = handleC2Connection(conn, buffer);
        break;
    default:
        break;
    }

    if (connStatus_[name] == Status::ConnectError)
    {
        connStatus_.erase(name);
        connC1Cache.erase(name);
        connS1Cache.erase(name);
        conn->shutdown();
    }
}

} // namespace rtmp

//长连接 要求客户端关闭连接时能检测到
int main(int argc, char **argv)
{
    if (argc != 2)
    {
        log_error("Usage : ./echo-server <port>");
        exit(-1);
    }

    rtmp::Server *rtmp = new rtmp::Server;
    EventLoop loop;
    INetAddr addr("", atoi(argv[1]));
    TcpServer server(&loop, addr, "EchoServer");

    server.set_onMessageCallback(std::bind(&rtmp::Server::onMessage, rtmp, _1, _2, _3));

    server.start();
    loop.loop();
}