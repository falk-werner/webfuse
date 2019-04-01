#ifndef WF_ADAPTER_IMPL_SESSION_H
#define WF_ADAPTER_IMPL_SESSION_H

#ifndef __cplusplus
#include <stdbool.h>
#include <stddef.h>
#else
#include <cstddef>
using std::size_t;
#endif

#include "webfuse/core/message_queue.h"
#include "webfuse/adapter/impl/jsonrpc/proxy.h"
#include "webfuse/adapter/impl/jsonrpc/server.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct lws;
struct wf_message;
struct wf_credentials;
struct wf_impl_authenticators;
struct wf_impl_timeout_manager;

struct wf_impl_session
{
    struct lws * wsi;
    bool is_authenticated;
    struct wf_message_queue queue;
    struct wf_impl_authenticators * authenticators;
    struct wf_impl_jsonrpc_server * server;
    struct wf_impl_jsonrpc_proxy rpc;
};

extern void wf_impl_session_init(
    struct wf_impl_session * session,
    struct lws * wsi,
   struct wf_impl_authenticators * authenticators,
   struct wf_impl_timeout_manager * timeout_manager,
   struct wf_impl_jsonrpc_server * server);

extern bool wf_impl_session_authenticate(
    struct wf_impl_session * session,
    struct wf_credentials * creds);

extern void wf_impl_session_receive(
    struct wf_impl_session * session,
    char const * data,
    size_t length);

extern void wf_impl_session_onwritable(
    struct wf_impl_session * session);

extern void wf_impl_session_cleanup(
    struct wf_impl_session * session);

#ifdef __cplusplus
}
#endif


#endif
