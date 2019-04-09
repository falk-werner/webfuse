#include "webfuse/adapter/impl/session_manager.h"
#include "webfuse/core/util.h"
#include "webfuse/core/container_of.h"
#include <stddef.h>

void wf_impl_session_manager_init(
    struct wf_impl_session_manager * manager)
{
    wf_dlist_init(&manager->sessions);
}

static void wf_impl_session_manager_cleanup_session(
    struct wf_dlist_item * item,
    void * user_data)
{
    (void) user_data;
    struct wf_impl_session * session = WF_CONTAINER_OF(item, struct wf_impl_session, item);

    wf_impl_session_dispose(session);
}

 

void wf_impl_session_manager_cleanup(
    struct wf_impl_session_manager * manager)
{
    wf_dlist_cleanup(&manager->sessions, &wf_impl_session_manager_cleanup_session, NULL);
}

struct wf_impl_session * wf_impl_session_manager_add(
    struct wf_impl_session_manager * manager,
    struct lws * wsi,
    struct wf_impl_authenticators * authenticators,
    struct wf_impl_timeout_manager * timeout_manager,
    struct wf_impl_jsonrpc_server * server,
    char const * mount_point,
    char const * protocol_name)
{
    struct wf_impl_session * session = wf_impl_session_create(
        wsi, authenticators, timeout_manager, server, mount_point, protocol_name); 
    if (NULL != session)
    {
        wf_dlist_prepend(&manager->sessions, &session->item);
    }

    return session;
}

static bool wf_impl_session_manager_get_predicate(
    struct wf_dlist_item * item,
    void * user_data)
{
    struct lws * wsi = user_data;
    struct wf_impl_session * session = WF_CONTAINER_OF(item, struct wf_impl_session, item);

    return ((wsi == session->wsi) || (wsi == session->wsi_fuse));
}

struct wf_impl_session * wf_impl_session_manager_get(
    struct wf_impl_session_manager * manager,
    struct lws * wsi)
{
    struct wf_impl_session * session = NULL;
    struct wf_dlist_item * item = wf_dlist_find_first(
        &manager->sessions, &wf_impl_session_manager_get_predicate, wsi);
    if (NULL != item)
    {
        session = WF_CONTAINER_OF(item, struct wf_impl_session, item);
    }

    return session;
}

static bool wf_impl_session_manager_remove_predicate(
    struct wf_dlist_item * item,
    void * user_data)
{
    struct lws * wsi = user_data;
    struct wf_impl_session * session = WF_CONTAINER_OF(item, struct wf_impl_session, item);

    return (wsi == session->wsi);
}

void wf_impl_session_manager_remove(
    struct wf_impl_session_manager * manager,
    struct lws * wsi)
{
    struct wf_impl_session * session = NULL;
    struct wf_dlist_item * item = wf_dlist_find_first(
        &manager->sessions, &wf_impl_session_manager_remove_predicate, wsi);
    if (NULL != item)
    {
        wf_dlist_remove(&manager->sessions, item);
        session = WF_CONTAINER_OF(item, struct wf_impl_session, item);
        wf_impl_session_dispose(session);
    }
}
