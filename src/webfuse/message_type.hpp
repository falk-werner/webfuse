#ifndef WEBFUSE_MESSAGETYPE_HPP
#define WEBFUSE_MESSAGETYPE_HPP

#include <cinttypes>

namespace webfuse
{

enum class message_type: uint8_t
{
    access_req   = 0x01,
    getattr_req  = 0x02,
    readlink_req = 0x03,
    symlink_req  = 0x04,
    link_req     = 0x05,
    rename_req   = 0x06,
    chmod_req    = 0x07,
    chown_req    = 0x08,
    truncate_req = 0x09,
    fsync_req    = 0x0a,
    open_req     = 0x0b,
    mknod_req    = 0x0c,
    create_req   = 0x0d,
    release_req  = 0x0e,
    unlink_req   = 0x0f,
    read_req     = 0x10,
    write_req    = 0x11,
    mkdir_req    = 0x12,
    readdir_req  = 0x13,
    rmdir_req    = 0x14,
    statfs_req   = 0x15 
};


}

#endif
