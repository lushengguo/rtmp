#include "client.h"
#include "base/timestamp.hpp"
#include "connection.h"
#include "rtmp.h"
#include <cassert>
#include <cstring>
#include <ctime>
#include <strings.h>

namespace rtmp {
// +--------------+                              +-------------+
// |    Client    |              |               |    Server   |
// +------+-------+              |               +------+------+
//        |               Handshaking done              |
//        |                      |                      |
//        |                      |                      |
//        |----------- Command Message(connect) ------->|
//        |                                             |
//        |<------- Window Acknowledgement Size --------|
//        |                                             |
//        |<----------- Set Peer Bandwidth -------------|
//        |                                             |
//        |-------- Window Acknowledgement Size ------->|
//        |                                             |
//        |<------ User Control Message(StreamBegin) ---|
//        |                                             |
//        |<------------ Command Message ---------------|
//        |        (_result- connect response)          |
//        |                                             |
void Client::handleS01Connection(reactor::TcpConnectionPtr conn, reactor::Buffer &buffer)
{
    // todo 重连，现在默认是服务端正常返回的
    if (buffer.readable_bytes() != sizeof(S0) + sizeof(S1))
        return;

    const char *data = buffer.readable_data();
    const S0 *s0 = (const S0 *)data;
    if (s0->version != 3)
    {
        assert(false);
        exit(-1);
    }

    data += sizeof(S0);
    const S1 *s1 = (const S1 *)data;
    C2 c2;
    memcpy(&c2, s1, sizeof(C2));
    c2.epochTime2 = time(nullptr);
    conn->send((const char *)&c2, sizeof(C2));
    status_ = Status::WaitForS2;
}

void Client::handleS2Connection(reactor::TcpConnectionPtr conn, reactor::Buffer &buffer)
{
    if (buffer.readable_bytes() != sizeof(S2))
        return;

    const char *data = buffer.readable_data();
    const S2 *s2 = (const S2 *)data;
    if (s2->epochTime == c1_->epochTime && memcmp(s2->randomEcho, c1_->randomBytes, sizeof(c1_->randomBytes)) == 0)
    {
        delete c1_;
        status_ = Status::ConnectionEstablished;
    }
    else
    {
        assert(false);
        exit(-1);
    }
}

void Client::sendC01Connection(reactor::TcpConnectionPtr conn)
{
    // handshake
    C0 c0;
    c0.version = 3;
    conn->send((const char *)&c0, sizeof(c0));

    if (!c1_)
        c1_ = new C1();
    bzero(c1_, sizeof(C1));
    c1_->epochTime = time(nullptr);
    conn->send((const char *)c1_, sizeof(C1));
}

bool call();

//      +-------------+                             +----------+
//      | Play Client |             |               |  Server  |
//      +------+------+             |               +-----+----+
//             |        Handshaking and Application       |
//             |               connect done               |
//             |                    |                     |
//             |                    |                     |
//    ---+---- |----- Command Message(createStream) ----->|
// Create|     |                                          |
// Stream|     |                                          |
//    ---+---- |<---------- Command Message --------------|
//             |    (_result- createStream response)      |
//             |                                          |
//    ---+---- |------ Command Message (play) ----------->|
//       |     |                                          |
//       |     |<------------ SetChunkSize ---------------|
//       |     |                                          |
//       |     |<---- User Control (StreamIsRecorded) ----|
//  Play |     |                                          |
//       |     |<------ UserControl (StreamBegin) --------|
//       |     |                                          |
//       |     |<- Command Message(onStatus-play reset) --|
//       |     |                                          |
//       |     |<- Command Message(onStatus-play start) --|
//       |     |                                          |
//       |     |<------------ Audio Message --------------|
//       |     |                                          |
//       |     |<------------ Video Message --------------|
//       |     |                     |                    |
//                                   |
//         Keep receiving audio and video stream till finishes
bool play();

//     +--------------+                          +-------------+
//     | Play2 Client |           |              |    Server   |
//     +--------+-----+           |              +------+------+
//              |      Handshaking and Application      |
//              |            connect done               |
//              |                 |                     |
//              |                 |                     |
//     ---+---- |---- Command Message(createStream) --->|
// Create |     |                                       |
// Stream |     |                                       |
//     ---+---- |<---- Command Message (_result) -------|
//              |                                       |
//     ---+---- |------ Command Message (play) -------->|
//        |     |                                       |
//        |     |<---------- SetChunkSize --------------|
//        |     |                                       |
//        |     |<--- UserControl (StreamIsRecorded) ---|
//   Play |     |                                       |
//        |     |<------ UserControl (StreamBegin) -----|
//        |     |                                       |
//        |     |<- Command Message(onStatus-playstart)-|
//        |     |                                       |
//        |     |<---------- Audio Message -------------|
//        |     |                                       |
//        |     |<---------- Video Message -------------|
//        |     |                                       |
//              |                                       |
//     ---+---- |-------- Command Message(play2) ------>|
//        |     |                                       |
//        |     |<------- Audio Message (new rate) -----|
//  Play2 |     |                                       |
//        |     |<------- Video Message (new rate) -----|
//        |     |                  |                    |
//        |     |                  |                    |
//        |  Keep receiving audio and video stream till finishes
//                                 |
bool play2();

void Client::onConnection(reactor::TcpConnectionPtr conn) { sendC01Connection(conn); }

void Client::onMessage(reactor::TcpConnectionPtr conn, reactor::Buffer &buffer, reactor::MicroTimeStamp receive_timestamp)
{
    switch (status_)
    {
    case Status::WaitForS01:
        handleS01Connection(conn, buffer);
        break;
    default:
        break;
    }
}

}  // namespace rtmp

//长连接 要求客户端关闭连接时能检测到
using namespace reactor;
using namespace std::placeholders;
int main(int argc, char **argv)
{
    rtmp::Client *rtmp = new rtmp::Client;
    EventLoop loop;
    INetAddr addr(argv[1], atoi(argv[2]));
    TcpClient client(&loop, addr, "EchoClient");

    client.set_onConnectionCallback(std::bind(&rtmp::Client::onConnection, rtmp, _1));
    client.set_onMessageCallback(std::bind(&rtmp::Client::onMessage, rtmp, _1, _2, _3));
    client.start();
    loop.loop();
}