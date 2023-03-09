#ifndef CLIENT_HPP
#define CLIENT_HPP
#include "base/noncopyable.hpp"
#include "connection.h"
#include "net/EventLoop.hpp"
#include "net/TcpClient.hpp"
namespace rtmp
{
class Client : reactor::noncopyable
{
  public:
    enum class Status
    {
        WaitForS01,
        WaitForS2,
        ConnectionEstablished,
    };

  public:
    Client() : status_(Status::WaitForS01) {}

    // NetConnection Command
    void handleS01Connection(reactor::TcpConnectionPtr conn, reactor::Buffer &buffer);
    void handleS2Connection(reactor::TcpConnectionPtr conn, reactor::Buffer &buffer);
    void sendC01Connection(reactor::TcpConnectionPtr conn);

    void call();
    void createStream();
    void close();

    // NetStream Command
    void play();
    void play2();
    void deleteStream();
    void closeStream();
    void receiveAudio();
    void receiveVideo();
    void publish();

  public:
    // net library callback
    void onConnection(reactor::TcpConnectionPtr conn);
    void onMessage(reactor::TcpConnectionPtr conn, reactor::Buffer &buffer, reactor::MicroTimeStamp receive_timestamp);

  private:
    Status status_;
    C1 *c1_ = nullptr;
};
} // namespace rtmp

#endif
