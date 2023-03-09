#ifndef CLIENT_HPP
#define CLIENT_HPP
namespace rtmp
{

class Client
{
  public:
    // NetConnection Command
    bool connect();
    bool call();
    bool createStream();
    bool close();

    // NetStream Command
    bool play();
    bool play2();
    bool deleteStream();
    bool closeStream();
    bool receiveAudio();
    bool receiveVideo();
    bool publish();

  private:
};
} // namespace rtmp

#endif
