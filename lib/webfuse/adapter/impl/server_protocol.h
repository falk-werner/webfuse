#ifndef WF_ADAPTER_IMPL_SERVER_PROTOCOL_H
#define WF_ADAPTER_IMPL_SERVER_PROTOCOL_H

#include "webfuse/adapter/impl/filesystem.h"
#include "webfuse/adapter/impl/jsonrpc/server.h"
#include "webfuse/adapter/impl/time/timeout_manager.h"
#include "webfuse/adapter/impl/authenticators.h"
#include "webfuse/adapter/impl/session_manager.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct lws_protocols;

struct wf_server_protocol
{
    struct wf_impl_timeout_manager timeout_manager;
    struct wf_impl_filesystem filesystem;
    struct wf_impl_jsonrpc_server rpc;
    struct wf_impl_authenticators authenticators;
    struct wf_impl_session_manager session_manager;
};

extern bool wf_impl_server_protocol_init(
    struct wf_server_protocol * protocol,
    char * mount_point);

extern void wf_impl_server_protocol_cleanup(
    struct wf_server_protocol * protocol);

extern struct wf_server_protocol * wf_impl_server_protocol_create(
    char * mount_point);

extern void wf_impl_server_protocol_dispose(
    struct wf_server_protocol * protocol);

extern void wf_impl_server_protocol_init_lws(
    struct wf_server_protocol * protocol,
    struct lws_protocols * lws_protocol);

extern void wf_impl_server_protocol_add_authenticator(
    struct wf_server_protocol * protocol,
    char const * type,
    wf_authenticate_fn * authenticate,
    void * user_data);

#ifdef __cplusplus
}
#endif

#endif
