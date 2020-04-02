#include "webfuse/adapter/impl/session_manager.h"
#include "webfuse/core/util.h"
#include "webfuse/core/container_of.h"
#include <stddef.h>

void wf_impl_session_manager_init(
    struct wf_impl_session_manager * manager)
{
    wf_slist_init(&manager->sessions);
}

void wf_impl_session_manager_cleanup(
    struct wf_impl_session_manager * manager)
{
    struct wf_slist_item * item = wf_slist_first(&manager->sessions);
    while (NULL != item)
    {
        struct wf_slist_item * next = item->next;
        struct wf_impl_session * session = wf_container_of(item, struct wf_impl_session, item);
        wf_impl_session_dispose(session);

        item = next;
    }
}

struct wf_impl_session * wf_impl_session_manager_add(
    struct wf_impl_session_manager * manager,
    struct lws * wsi,
    struct wf_impl_authenticators * authenticators,
    struct wf_impl_mountpoint_factory * mountpoint_factory,
    struct wf_timer_manager * timer_manager,
    struct wf_jsonrpc_server * server)
{
    struct wf_impl_session * session = wf_impl_session_create(
        wsi, authenticators, timer_manager, server, mountpoint_factory); 
    wf_slist_append(&manager->sessions, &session->item);

    return session;
}

struct wf_impl_session * wf_impl_session_manager_get(
    struct wf_impl_session_manager * manager,
    struct lws * wsi)
{
    struct wf_impl_session * session = NULL;

    struct wf_slist_item * item = wf_slist_first(&manager->sessions);
    while (NULL != item)
    {
        struct wf_slist_item * next = item->next;
        struct wf_impl_session * current = wf_container_of(item, struct wf_impl_session, item);

        if (wf_impl_session_contains_wsi(current, wsi)) {
            session = current;
            break;
        }

        item = next;
    }

    return session;
}

void wf_impl_session_manager_remove(
    struct wf_impl_session_manager * manager,
    struct lws * wsi)
{
    struct wf_slist_item * prev = &manager->sessions.head;
    while (NULL != prev->next)
    {
        struct wf_slist_item * item = prev->next;
        struct wf_impl_session * session = wf_container_of(item, struct wf_impl_session, item);
        if (wsi == session->wsi)
        {
            wf_slist_remove_after(&manager->sessions, prev);
            wf_impl_session_dispose(session);
            break;
        }

        prev = prev->next;
    }
}
