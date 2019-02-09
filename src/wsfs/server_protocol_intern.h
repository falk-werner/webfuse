#ifndef _WSFS_SERVER_PROTOCOL_INTERN_H
#define _WSFS_SERVER_PROTOCOL_INTERN_H

#include "wsfs/server_protocol.h"
#include "wsfs/filesystem.h"
#include "wsfs/message_queue.h"
#include "wsfs/jsonrpc/server.h"

struct wsfs_server_protocol
{
    struct wsfs_filesystem filesystem;
    struct wsfs_jsonrpc_server rpc;
    struct wsfs_message_queue queue;
    struct lws * wsi;    
};

extern bool wsfs_server_protocol_init(
    struct wsfs_server_protocol * protocol,
    char * mount_point);

extern void wsfs_server_protocol_cleanup(
    struct wsfs_server_protocol * protocol);

#endif
