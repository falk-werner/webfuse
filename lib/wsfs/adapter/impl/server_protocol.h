#ifndef WSFS_ADAPTER_IMPL_SERVER_PROTOCOL_H
#define WSFS_ADAPTER_IMPL_SERVER_PROTOCOL_H

#include "wsfs/adapter/impl/filesystem.h"
#include "wsfs/adapter/impl/jsonrpc/server.h"
#include "wsfs/adapter/impl/time/timeout_manager.h"
#include "wsfs/adapter/impl/authenticators.h"
#include "wsfs/adapter/impl/session_manager.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct lws_protocols;

struct wsfs_server_protocol
{
    struct wsfs_timeout_manager timeout_manager;
    struct wsfs_filesystem filesystem;
    struct wsfs_jsonrpc_server rpc;
    struct wsfs_authenticators authenticators;
    struct wsfs_session_manager session_manager;
};

extern bool wsfs_impl_server_protocol_init(
    struct wsfs_server_protocol * protocol,
    char * mount_point);

extern void wsfs_impl_server_protocol_cleanup(
    struct wsfs_server_protocol * protocol);

extern struct wsfs_server_protocol * wsfs_impl_server_protocol_create(
    char * mount_point);

extern void wsfs_impl_server_protocol_dispose(
    struct wsfs_server_protocol * protocol);

extern void wsfs_impl_server_protocol_init_lws(
    struct wsfs_server_protocol * protocol,
    struct lws_protocols * lws_protocol);

extern void wsfs_impl_server_protocol_add_authenticator(
    struct wsfs_server_protocol * protocol,
    char const * type,
    wsfs_authenticate_fn * authenticate,
    void * user_data);

#ifdef __cplusplus
}
#endif

#endif
