#ifndef WF_ADAPTER_IMPL_SERVER_PROTOCOL_H
#define WF_ADAPTER_IMPL_SERVER_PROTOCOL_H

#include "webfuse/adapter/impl/authenticators.h"
#include "webfuse/adapter/impl/mountpoint_factory.h"
#include "webfuse/adapter/impl/session_manager.h"
#include "webfuse/core/jsonrpc/proxy.h"
#include "webfuse/core/jsonrpc/server.h"

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

struct lws_protocols;
struct wf_timer_manager;

struct wf_server_protocol
{
    struct wf_impl_authenticators authenticators;
    struct wf_impl_mountpoint_factory mountpoint_factory;
    struct wf_impl_session_manager session_manager;
    struct wf_jsonrpc_server * server;
    struct wf_timer_manager * timer_manager;
    bool is_operational;
};

extern void wf_impl_server_protocol_init(
    struct wf_server_protocol * protocol,
    struct wf_impl_mountpoint_factory * mountpoint_factory);

extern void wf_impl_server_protocol_cleanup(
    struct wf_server_protocol * protocol);

extern WF_API struct wf_server_protocol * wf_impl_server_protocol_create(
    wf_create_mountpoint_fn * create_mountpoint,
    void * create_mountpoint_context);

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
