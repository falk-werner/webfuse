#ifndef WSFS_ADAPTER_SERVER_PROTOCOL_INTERN_H
#define WSFS_ADAPTER_SERVER_PROTOCOL_INTERN_H

#include "wsfs/adapter/server_protocol.h"
#include "wsfs/adapter/filesystem.h"
#include "wsfs/adapter/jsonrpc/server.h"
#include "wsfs/adapter/time/timeout_manager.h"
#include "wsfs/adapter/authenticators.h"
#include "wsfs/adapter/session_manager.h"

struct wsfs_server_protocol
{
    struct wsfs_timeout_manager timeout_manager;
    struct wsfs_filesystem filesystem;
    struct wsfs_jsonrpc_server rpc;
    struct wsfs_authenticators authenticators;
    struct wsfs_session_manager session_manager;
};

extern bool wsfs_server_protocol_init(
    struct wsfs_server_protocol * protocol,
    char * mount_point);

extern void wsfs_server_protocol_cleanup(
    struct wsfs_server_protocol * protocol);

#endif
