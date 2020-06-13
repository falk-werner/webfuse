#ifndef WF_ADAPTER_IMPL_CLIENT_PROTOCOL_H
#define WF_ADAPTER_IMPL_CLIENT_PROTOCOL_H

#include "webfuse/adapter/client_callback.h"
#include "webfuse/core/slist.h"

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

struct lws_protocols;
struct lws_context;

struct wf_impl_filesystem;
struct wf_jsonrpc_proxy;
struct wf_timer_manager;

typedef void
wf_client_protocol_callback_fn(
    void * user_data,
    int reason,
    void * arg);

struct wf_client_protocol
{
    bool is_connected;
    bool is_shutdown_requested;
    struct lws * wsi;
    wf_client_protocol_callback_fn * callback;
    struct wf_impl_filesystem * filesystem;
    void * user_data;
    struct wf_timer_manager * timer_manager;
    struct wf_jsonrpc_proxy * proxy;
    struct wf_slist messages;
};

extern void
wf_impl_client_protocol_init(
    struct wf_client_protocol * protocol,
    wf_client_protocol_callback_fn * callback,
    void * user_data);

extern void
wf_impl_client_protocol_cleanup(
    struct wf_client_protocol * protocol);

extern void
wf_impl_client_protocol_callback(
    struct wf_client_protocol * protocol,
    int reason,
    void * arg);

extern void
wf_impl_client_protocol_init_lws(
    struct wf_client_protocol * protocol,
    struct lws_protocols * lws_protocol);

extern void
wf_impl_client_protocol_connect(
    struct wf_client_protocol * protocol,
    struct lws_context * conext,
    char const * url);

extern void
wf_impl_client_protocol_disconnect(
    struct wf_client_protocol * protocol);

extern void
wf_impl_client_protocol_authenticate(
    struct wf_client_protocol * protocol);

extern void
wf_impl_client_protocol_add_filesystem(
    struct wf_client_protocol * protocol,
    char const * local_path,
    char const * name);

#ifdef __cplusplus
}
#endif


#endif
