#ifndef WF_ADAPTER_IMPL_SESSION_MANAGER_H
#define WF_ADAPTER_IMPL_SESSION_MANAGER_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#include "webfuse/adapter/impl/session.h"
#include "webfuse/adapter/impl/fuse_wrapper.h"
#include "webfuse/core/slist.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct lws;
struct wf_timer_manager;
struct wf_jsonrpc_server;

struct wf_impl_session_manager
{
    struct wf_slist sessions;
};

extern void wf_impl_session_manager_init(
    struct wf_impl_session_manager * manager);

extern void wf_impl_session_manager_cleanup(
    struct wf_impl_session_manager * manager);

extern struct wf_impl_session * wf_impl_session_manager_add(
    struct wf_impl_session_manager * manager,
    struct lws * wsi,
    struct wf_impl_authenticators * authenticators,
    struct wf_impl_mountpoint_factory * mountpoint_factory,
    struct wf_timer_manager * timer_manager,
    struct wf_jsonrpc_server * server);

extern struct wf_impl_session * wf_impl_session_manager_get(
    struct wf_impl_session_manager * manager,
    struct lws * wsi);

extern void wf_impl_session_manager_remove(
    struct wf_impl_session_manager * manager,
    struct lws * wsi);

#ifdef __cplusplus
}
#endif

#endif