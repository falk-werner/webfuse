#ifndef WEBFUSE_RESPONSE_TYPE
#define WEBFUSE_RESPONSE_TYPE

#include <cinttypes>

namespace webfuse
{

enum class response_type: uint8_t
{
    unknown  = 0x80,
    access   = 0x81,
    getattr  = 0x82,
    readlink = 0x83,
    symlink  = 0x84,
    link     = 0x85,
    rename   = 0x86,
    chmod    = 0x87,
    chown    = 0x88,
    truncate = 0x89,
    fsync    = 0x8a,
    open     = 0x8b,
    mknod    = 0x8c,
    create   = 0x8d,
    release  = 0x8e,
    unlink   = 0x8f,
    read     = 0x90,
    write    = 0x91,
    mkdir    = 0x92,
    readdir  = 0x93,
    rmdir    = 0x94,
    statfs   = 0x95,
    utimens  = 0x96
};

}

#endif