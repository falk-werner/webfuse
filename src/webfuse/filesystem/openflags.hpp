#ifndef WEBFUSE_OPENFLAGS_HPP
#define WEBFUSE_OPENFLAGS_HPP

#include <cinttypes>

namespace webfuse
{

class openflags
{
public:
    static constexpr int32_t const accessmode_mask = 03; // O_ACCMODE
    static constexpr int32_t const rdonly          =  0; // O_RDONLY
    static constexpr int32_t const wronly          = 01; // O_WRONLY
    static constexpr int32_t const rdwr            = 02; // O_RDWR

    static constexpr int32_t const append    =     02000; // O_APPEND
    static constexpr int32_t const async     =    020000; // O_ASYNC
    static constexpr int32_t const cloexec   =  02000000; // O_CLOEXEC
    static constexpr int32_t const creat     =      0100; // O_CREAT
    static constexpr int32_t const direct    =    040000; // O_DIRECT
    static constexpr int32_t const directory =   0200000; // O_DIRECTORY
    static constexpr int32_t const dsync     =    010000; // O_DSYNC
    static constexpr int32_t const excl      =      0200; // O_EXCL
    static constexpr int32_t const largefile =   0100000; // O_LARGEFILE
    static constexpr int32_t const noatime   =  01000000; // O_NOATIME
    static constexpr int32_t const noctty    =      0400; // O_NOCTTY
    static constexpr int32_t const nofollow  =   0400000; // O_NOFOLLOW
    static constexpr int32_t const nonblock  =     04000; // O_NONBLOCK
    static constexpr int32_t const ndelay    =     04000; // O_NDELAY
    static constexpr int32_t const path      = 010000000; // O_PATH
    static constexpr int32_t const sync      =  04010000; // O_SYNC
    static constexpr int32_t const tmpfile   = 020200000; // O_TMPFILE
    static constexpr int32_t const trunc     =     01000; // O_TRUNC

    explicit openflags(int32_t value = 0);
    operator int32_t() const;

    static openflags from_int(int value);
    int to_int() const;
private:
    int32_t value_;
};

}

#endif
