#ifndef SERVER_HPP
#define SERVER_HPP
#include "net/EventLoop.hpp"
#include "net/TcpServer.hpp"
#include <cstdint>
#include "connection.h"

namespace rtmp {
class Server
{
  public:
    enum class Status
    {
        ConnectWaitForC01,
        ConnectWaitForC2,
        ConnectError,
        ConnectionEstablished,
    };

  public:
    void onMessage(reactor::TcpConnectionPtr conn, reactor::Buffer &buffer, reactor::MicroTimeStamp receive_timestamp);
    void onConnection(reactor::TcpConnectionPtr conn);

  private:
    Status handleC01Connection(reactor::TcpConnectionPtr conn, reactor::Buffer &buffer);
    Status handleC2Connection(reactor::TcpConnectionPtr conn, reactor::Buffer &buffer);

  private:
    std::map<std::string, Status> connStatus_;
    std::map<std::string, C1> connC1Cache;
    std::map<std::string, S1> connS1Cache;
};
}  // namespace rtmp

#endif
