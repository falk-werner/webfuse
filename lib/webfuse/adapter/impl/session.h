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
#include "webfuse/adapter/impl/filesystem.h"
#include "webfuse/core/slist.h"

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
    struct wf_slist_item item;
    char * mount_point;
    struct lws * wsi;
    bool is_authenticated;
    struct wf_slist messages;
    struct wf_impl_authenticators * authenticators;
    struct wf_impl_jsonrpc_server * server;
    struct wf_impl_jsonrpc_proxy rpc;
    struct wf_slist filesystems;
};

extern struct wf_impl_session * wf_impl_session_create(
    struct lws * wsi,
    struct wf_impl_authenticators * authenticators,
    struct wf_impl_timeout_manager * timeout_manager,
    struct wf_impl_jsonrpc_server * server,
    char const * mount_point);

extern void wf_impl_session_dispose(
    struct wf_impl_session * session);

extern bool wf_impl_session_authenticate(
    struct wf_impl_session * session,
    struct wf_credentials * creds);

extern bool wf_impl_session_add_filesystem(
    struct wf_impl_session * session,
    char const * name);

extern void wf_impl_session_receive(
    struct wf_impl_session * session,
    char const * data,
    size_t length);

extern void wf_impl_session_onwritable(
    struct wf_impl_session * session);

extern bool wf_impl_session_contains_wsi(
    struct wf_impl_session * session,
    struct lws * wsi);

extern void wf_impl_session_process_filesystem_request(
    struct wf_impl_session * session, 
    struct lws * wsi);


#ifdef __cplusplus
}
#endif


#endif
