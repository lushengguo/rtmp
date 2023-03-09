#include <cstdint>
#include <stdint.h>

namespace rtmp
{
struct C0
{
    uint64_t version;
};

struct S0 : C0
{
};

struct __attribute__((__packed__)) C1
{
    uint64_t epochTime;
    uint64_t zero;
    char randomBytes[1528];
};

struct S1 : C1
{
};

struct __attribute__((__packed__)) C2
{
    uint64_t epochTime;
    uint64_t epochTime2;
    char randomEcho[1528];
};

struct S2 : C2
{
};
} // namespace rtmp