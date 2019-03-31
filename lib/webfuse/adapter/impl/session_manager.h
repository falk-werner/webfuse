#ifndef WF_ADAPTER_IMPL_SESSION_MANAGER_H
#define WF_ADAPTER_IMPL_SESSION_MANAGER_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#include "webfuse/adapter/impl/session.h"
#include "webfuse/adapter/impl/fuse_wrapper.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct lws;
struct wf_impl_timeout_manager;
struct wf_impl_jsonrpc_server;

struct wf_impl_session_manager
{
    struct wf_impl_session session;
};

extern void wf_impl_session_manager_init(
    struct wf_impl_session_manager * manager);

extern void wf_impl_session_manager_cleanup(
    struct wf_impl_session_manager * manager);

extern struct wf_impl_session * wf_impl_session_manager_add(
    struct wf_impl_session_manager * manager,
    struct lws * wsi,
    struct wf_impl_authenticators * authenticators,
    struct wf_impl_timeout_manager * timeout_manager,
    struct wf_impl_jsonrpc_server * server);

extern struct wf_impl_session * wf_impl_session_manager_get(
    struct wf_impl_session_manager * manager,
    struct lws * wsi);

extern struct wf_impl_session * wf_impl_session_manager_get_by_inode(
    struct wf_impl_session_manager * session_manger,
    fuse_ino_t inode);

extern void wf_impl_session_manager_remove(
    struct wf_impl_session_manager * manager,
    struct lws * wsi);

#ifdef __cplusplus
}
#endif

#endif