#include "client.h"

#include "rtmp.h"

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
bool connect();

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
}  // namespace rtmp