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

struct server_protocol
{
    struct timeout_manager timeout_manager;
    struct filesystem filesystem;
    struct jsonrpc_server rpc;
    struct authenticators authenticators;
    struct session_manager session_manager;
};

extern bool server_protocol_init(
    struct server_protocol * protocol,
    char * mount_point);

extern void server_protocol_cleanup(
    struct server_protocol * protocol);

extern struct server_protocol * server_protocol_create(
    char * mount_point);

extern void server_protocol_dispose(
    struct server_protocol * protocol);

extern void server_protocol_init_lws(
    struct server_protocol * protocol,
    struct lws_protocols * lws_protocol);

extern void server_protocol_add_authenticator(
    struct server_protocol * protocol,
    char const * type,
    authenticate_fn * authenticate,
    void * user_data);

#ifdef __cplusplus
}
#endif

#endif
