#ifndef _WSFS_SERVER_PROTOCOL_INTERN_H
#define _WSFS_SERVER_PROTOCOL_INTERN_H

#include "wsfs/server_protocol.h"
#include "wsfs/filesystem.h"

struct wsfs_server_protocol
{
    struct wsfs_filesystem filesystem;
};

extern void wsfs_server_protocol_init(
    struct wsfs_server_protocol * protocol,
    char * mount_point);

extern void wsfs_server_protocol_cleanup(
    struct wsfs_server_protocol * protocol);

#endif
