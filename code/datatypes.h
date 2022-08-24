#if !defined(DATATYPES_H)
#include <stdint.h>

#define global        static
#define internal      static
#define local_persist static

#define KiB(k) (1024LL*(k))
#define MiB(m) (1024LL*KiB(m))
#define GiB(g) (1024LL*MiB(g))
#define TiB(t) (1024LL*GiB(t))
#define KB(k)  (1000LL*(k))
#define MB(m)  (1000LL*KB(m))
#define GB(g)  (1000LL*MB(g))
#define TB(t)  (1000LL*GB(t))

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef s32     b32;

typedef size_t Memory_Index;

typedef float  r32;
typedef double r64;

union v2i
{
    struct { s32 x, y; };
    s32    row[2];
};

union v2
{
    struct { r32 x, y; };
    r32    row[2];
};

union v3
{
    struct { r32 x, y, z; };
    r32    row[3];
    v2     xy;

    operator bool()
    {
        return x||y||z;
    }

};

union v4
{
    struct { r32 x, y, z, w; };
    struct { v3 xyz; r32 _w; };
    r32    row[4];
};

union m4
{
    r32 m[4][4];
    v4  col [4];
};

#define DATATYPES_H
#endif
