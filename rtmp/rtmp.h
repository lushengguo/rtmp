#ifndef RTMP_HPP
#define RTMP_HPP
#include <map>
#include <string>

namespace rtmp
{
// 最完整的 RTMP Chunk Header 长度是 12 字节。
struct __attribute__((__packed__)) ChunkHeader12
{
    struct
    {
        uint8_t type : 2;
        uint8_t streamId : 6;
    };

    uint8_t timestamp[3];
    uint8_t messageLength[3];
    uint8_t messageTypeId;
    uint32_t messageStreamId;
};

// 一般情况下，msg stream id 是不会变的，所以针对视频或音频，
// 除了第一个 Chunk 的 RTMP Chunk Header 是 12 字节的，
// 后续的 Chunk 可省略这个 4 字节的字段，采用 8 字节的 RTMP Chunk Header。
struct __attribute__((__packed__)) ChunkHeader8
{
    struct
    {
        uint8_t type : 2;
        uint8_t streamId : 6;
    };

    uint8_t timestamp[3];
    uint8_t messageLength[3];
    uint8_t messageTypeId;
};

// 如果和前一条 Chunk 相比，当前 Chunk 的消息长度 message length 和消息类型 msg
// type id（视频为 9；音频为 8）字段又相同 即可将这两部分也省去，RTMP Chunk
// Header 采用 4 字节类型。
struct __attribute__((__packed__)) ChunkHeader4
{
    struct
    {
        uint8_t type : 2;
        uint8_t streamId : 6;
    };

    uint8_t timestamp[3];
    uint8_t messageLength[3];
};

// 如果和前一条 Chunk 相比，当前 Chunk 的 msg stream id、msg type id、message
// length 字段都相同 而且都属于同一个消息（由同一个 Message 切割而来），那么这些
// Chunk 的时间戳 timestamp 字段也会是相同的 故后面的 Chunk 也可以省去 timestamp
// 字段，RTMP Chunk Header 采用 1 字节类型。
struct __attribute__((__packed__)) ChunkHeader1
{
    uint8_t type : 2;
    uint8_t streamId : 6;
};

enum class MessageType
{
    SetChunkSize = 1,
    Abort = 2,
    Acknowledgement = 3,
    WindowAcknowledgementSize = 5,
    SetPeerBandWidth = 6,

    UserControl = 4,

    AudioMessage = 8,
    VideoMessage = 9,
    AggregateMessage = 22,

    // CommandMessage
    C_AMF3Encode = 17,
    C_AMF20Encode = 20,

    // DataMessage
    D_AMF0Encode = 18,
    D_AMF3Encode = 15,

    // SharedObjectMessage
    S_AMF0Encode = 19,
    S_AMF3Encode = 16,
};

enum class UserControlMessageEvents
{
    StreamBegin = 0,
    StreamEnd = 1,
    streamDry = 2,
    SetBufferLength = 3,
    StreamIsRecorded = 4,
    PingRequest = 6,
    PingResponse = 7
};

enum class SharedObjectMessageEvents
{
    Use = 1,           //	客户端向服务端发送，请求创建指定名称的共享对象。
    Release = 2,       //	客户端通知服务端，共享对象已在本地删除。
    RequestChange = 3, //	客户端请求修改共享对象的属性值。
    Change = 4,        //	服务端向除请求发送方外的其他客户端发送，通知其有属性的值发生了变化。
    Success = 5,       //	“请求更新”事件被接受后，服务端向发送请求的客户端回复此事件。
    SendMessage = 6, //	客户端向服务端发送此事件，来广播一个消息。服务端收到此事件后向所有客户端广播一条消息，包括请求方客户端。
    Status = 7,      //	服务端发送此事件来通知客户端错误信息。
    Clear = 8, //	服务端向客户端发送此事件，通知客户端清除一个共享对象。服务端在回复客户端的“创建”事件时也会发送此事件。
    Remove = 9,         //	服务端发送此事件，使客户端删除一个插槽。
    RequestRemove = 10, //	客户端删除一个插槽时发送此事件。
    UseSuccess = 11,    //	当连接成功时服务端向客户端发送此事件。
};

enum class NetConnectionCmd
{
    Connect,
    Call,
    CreateStream,
    Close
};

enum class Client2ServerCmd
{
    play,
    play2,
    deleteStream,
    closeStream,
    receiveAudio,
    receiveVideo,
    publish,
    seek,
    pause
};

enum class Server2ClietCmd
{
    onStatus
};

struct __attribute__((__packed__)) Message
{
    uint8_t type;
    uint8_t length[3];
    uint32_t timestamp;
    uint8_t streamId[3];
};

enum class AudioCodecs
{
    SUPPORT_SND_NONE = 0x0001,    // Raw sound, no compression
    SUPPORT_SND_ADPCM = 0x0002,   // ADPCM compression
    SUPPORT_SND_MP3 = 0x0004,     // mp3 compression
    SUPPORT_SND_INTEL = 0x0008,   // Not used
    SUPPORT_SND_UNUSED = 0x0010,  // Not used
    SUPPORT_SND_NELLY8 = 0x0020,  // NellyMoser at 8-kHz compression
    SUPPORT_SND_NELLY = 0x0040,   // NellyMoser compression (5, 11, 22, and 44 kHz)
    SUPPORT_SND_G711A = 0x0080,   // G711A sound compression (Flash Media Server only)
    SUPPORT_SND_G711U = 0x0100,   // G711U sound compression (Flash Media Server only)
    SUPPORT_SND_NELLY16 = 0x0200, // NellyMouser at 16-kHz compression
    SUPPORT_SND_AAC = 0x0400,     // Advanced audio coding (AAC) codec
    SUPPORT_SND_SPEEX = 0x0800,   // Speex Audio
    SUPPORT_SND_ALL = 0x0FFF      // All RTMP-supported audio codecs
};

enum class VideoCodec
{
    SUPPORT_VID_UNUSED = 0x0001,    // Obsolete value
    SUPPORT_VID_JPEG = 0x0002,      // Obsolete value
    SUPPORT_VID_SORENSON = 0x0004,  // Sorenson Flash video
    SUPPORT_VID_HOMEBREW = 0x0008,  // V1 screen sharing
    SUPPORT_VID_VP6 = 0x0010,       // (On2) On2 video (Flash 8+)
    SUPPORT_VID_VP6ALPHA = 0x0020,  // (On2 with alpha channel) On2 video with alpha channel
    SUPPORT_VID_HOMEBREWV = 0x0040, // (screensharing v2) Screen sharing version 2 (Flash8+)
    SUPPORT_VID_H264 = 0x0080,      // H264 video
    SUPPORT_VID_ALL = 0x00FF        // All RTMP-supported video codecs
};

enum class ViedeoFunction
{
    SUPPORT_VID_CLIENT_SEEK = 1 // Indicates that the client can perform frame-accurate seeks.
};

enum class ObjectEncoding
{
    AMF0 = 0, // AMF0 object encoding supported by Flash 6 and later.
    AMF3 = 3  // AMF3 encoding from Flash 9 (AS3).
};

struct ClientConnectParam
{
    std::string app;               // The Server application name the client is connected to.
    std::string flashver;          // Flash Player version. It is the same string as returned by the ApplicationScript getversion() function.
    std::string swfUrl;            // URL of the source SWF file making the connection.
    std::string tcUrl;             // URL of the Server. It has the following format. protocol: // servername:port/appName/appInstance
    bool fpad;                     // True if proxy is being used.
    AudioCodecs audioCodecs;       // Indicates what audio codecs the client supports.
    VideoCodec videoCodecs;        // Indicates what video codecs are supported.
    ViedeoFunction videoFunction;  // Indicates what special video functions are supported.
    std::string pageUrl;           // URL of the web page from where the SWF file was loaded.
    ObjectEncoding objectEncoding; // AMF encoding method.
};

typedef std::map<std::string, std::string> Object;
struct ServerConnectParam
{
    std::string commandName; // _result or _error; indicates whether the response is result or error.
    int transactionId;       // Transaction ID is 1 for connect responses.
    Object properties;       // Name-value pairs that describe the properties(fmsver etc.) of the connection.

    // Name-value pairs that describe the response from the server.
    // code, level, description are names of few among such information.
    Object infomation;
};

struct GeneralCommandParam
{
    std::string commandName; // Name of the remote procedure that is called.
    int transactionId;       // If a response is expected we give a transaction Id. Else we pass a value of 0.
    Object commandObject;    // If there exists any command info this is set, else this is set to null type.
};

struct ClientCallParam : GeneralCommandParam
{
    Object optionalArguments; // Any optional arguments to be provided.
};

struct ServerCallParam : GeneralCommandParam
{
    Object response; // Response from the method that was called.
};

struct ClientCreateStreamParam : GeneralCommandParam
{
};

struct ServerCreateStreamParam : GeneralCommandParam
{
    int streamId; // The return value is either a stream ID or an error information object.
};

struct ClientPlayParam : GeneralCommandParam
{
    // Name of the stream to play. To play video (FLV) files, specify the name of the stream without a file extension (for example, “sample”). To play
    // back MP3 or ID3 tags, you must precede the stream name with mp3: (for example, “mp3:sample”). To play H.264/AAC files, you must precede the
    // stream name with mp4: and specify the file extension. For example, to play the file sample.m4v, specify “mp4:sample.m4v”.
    std::string streamName;

    //	An optional parameter that specifies the start time in seconds. The default value is -2, which means the subscriber first tries to play the
    // live stream specified in the Stream Name field. If a live stream of that name is not found, it plays the recorded stream of the same name. If
    // there is no recorded stream with that name, the subscriber waits for a new live stream with that name and plays it when available. If you pass
    //-1 in the Start field, only the live stream specified in the Stream Name field is played. If you pass 0 or a positive number in the Start field,
    // a recorded stream specified in the Stream Name field is played beginning from the time specified in the Start field. If no recorded stream is
    // found, the next item in the playlist is played.
    int start;

    // An optional parameter that specifies the duration of playback in seconds. The default value is -1. The -1 value means a live stream is played
    // until it is no longer available or a recorded stream is played until it ends. If you pass 0, it plays the single frame since the time specified
    // in the Start field from the beginning of a recorded stream. It is assumed that the value specified in the Start field is equal to or greater
    // than 0. If you pass a positive number, it plays a live stream for the time period specified in the Duration field. After that it becomes
    // available or plays a recorded stream for the time specified in the Duration field. (If a stream ends before the time specified in the Duration
    // field, playback ends when the stream ends.) If you pass a negative number other than -1 in the Duration field, it interprets the value as if it
    // were -1.
    int duration;

    bool reset; // An optional Boolean value or number that specifies whether to flush any previous playlist.
};

struct ClientPlay2Param : GeneralCommandParam
{
    // An AMF encoded object whose properties are the public properties
    // described for the flash.net.NetStreamPlayOptions ActionScript object.
    Object parameters;
};

struct ClientDeleteStreamParam : GeneralCommandParam
{
    int streamId; // The ID of the stream that is destroyed on the server.
};

struct ClientReceiveAudioParam : GeneralCommandParam
{
    bool receiveAudio; //	true or false to indicate whether to receive audio or not.
};

struct ClientReceiveVideoParam : GeneralCommandParam
{
    bool receiveVideo; //	true or false to indicate whether to receive video or not.
};

struct ClientPublishParam : GeneralCommandParam
{
    std::string publishlingName; //	Name with which the stream is published.

    //	Type of publishing. Set to “live”, “record”, or “append”. record: The stream is published and the data is recorded to a new file. The file is
    // stored on the server in a subdirectory within the directory that contains the server application. If the file already exists, it is
    // overwritten. append: The stream is published and the data is appended to a file. If no file is found, it is created. live: Live data is
    // published without recording it in a file.
    std::string publishingType;
};

struct ClientSeekParam : GeneralCommandParam
{
    time_t milliSeconds; // Number of milliseconds to seek into the playlist.
};

struct ClientPauseParam : GeneralCommandParam
{
    bool pause;          // 	true or false, to indicate pausing or resuming play.
    time_t milliSeconds; // Number of milliseconds to seek into the playlist.
};

struct ClientOnStatusParam : GeneralCommandParam
{
    // An AMF object having at least the following three properties: level(String): the level for this message, one of “warning”, “status”, or
    // “error”; code (String): the message code, for example “NetStream.Play.Start”; and description (String): a human-readable description of the
    // message. The Info object MAY contain other properties as appropriate to the code.
    Object infoObject;
};

} // namespace rtmp
#endif