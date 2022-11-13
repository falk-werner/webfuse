#ifndef WEBFUSE_OPENFLAGS_HPP
#define WEBFUSE_OPENFLAGS_HPP

#include <cinttypes>

namespace webfuse
{

class openflags
{
public:
    static constexpr int32_t const accessmode_mask = 3; // O_ACCMODE

    static constexpr int32_t const rdonly    = 00000000; // O_RDONLY
    static constexpr int32_t const wronly    = 00000001; // O_WRONLY
    static constexpr int32_t const rdwr      = 00000002; // O_RDWR
    static constexpr int32_t const cloexec   = 02000000; // O_CLOEXEC
    static constexpr int32_t const creat     = 00000100; // O_CREAT
    static constexpr int32_t const direct    = 00040000; // O_DIRECT
    static constexpr int32_t const directory = 00200000; // O_DIRECTORY
    static constexpr int32_t const excl      = 00000200; // O_EXCL
    static constexpr int32_t const noctty    = 00000400; // O_NOCTTY
    static constexpr int32_t const nofollow  = 00400000; // O_NOFOLLOW
    static constexpr int32_t const trunc     = 00001000; // O_TRUNC
    static constexpr int32_t const async     = 00002000; // O_ASYNC
    static constexpr int32_t const largefile = 00000000; // O_LARGEFILE
    static constexpr int32_t const noatime   = 01000000; // O_NOATIME
    static constexpr int32_t const nonblock  = 00004000; // O_NONBLOCK
    static constexpr int32_t const ndelay    = 00004000; // O_NDELAY
    static constexpr int32_t const sync      = 04010000; // O_SYNC

    explicit openflags(int32_t value = 0);
    operator int32_t() const;

    static openflags from_int(int value);
    int to_int() const;
private:
    int32_t value_;
};

}

#endif
