#ifndef WEBFUSE_REQUEST_TYPE
#define WEBFUSE_REQUEST_TYPE

#include <cinttypes>

namespace webfuse
{

enum class request_type: uint8_t
{
    unknown  = 0x00,
    access   = 0x01,
    getattr  = 0x02,
    readlink = 0x03,
    symlink  = 0x04,
    link     = 0x05,
    rename   = 0x06,
    chmod    = 0x07,
    chown    = 0x08,
    truncate = 0x09,
    fsync    = 0x0a,
    open     = 0x0b,
    mknod    = 0x0c,
    create   = 0x0d,
    release  = 0x0e,
    unlink   = 0x0f,
    read     = 0x10,
    write    = 0x11,
    mkdir    = 0x12,
    readdir  = 0x13,
    rmdir    = 0x14,
    statfs   = 0x15,
    utimens  = 0x16,
    getcreds = 0x17
};

request_type get_request_type(uint8_t value);

}

#endif
