#include "webfuse/adapter/impl/session_manager.h"
#include "webfuse/core/util.h"
#include <stddef.h>

void wf_impl_session_manager_init(
    struct wf_impl_session_manager * manager)
{
    wf_impl_session_init(&manager->session, NULL, NULL, NULL);
}

void wf_impl_session_manager_cleanup(
    struct wf_impl_session_manager * manager)
{
    wf_impl_session_cleanup(&manager->session);
}

struct wf_impl_session * wf_impl_session_manager_add(
    struct wf_impl_session_manager * manager,
    struct lws * wsi,
    struct wf_impl_authenticators * authenticators,
    struct wf_impl_timeout_manager * timeout_manager)
{
    struct wf_impl_session * session = NULL; 
    if (NULL == manager->session.wsi)
    {
        session = &manager->session;
        wf_impl_session_init(&manager->session, wsi, authenticators, timeout_manager);        
    }

    return session;
}

struct wf_impl_session * wf_impl_session_manager_get(
    struct wf_impl_session_manager * manager,
    struct lws * wsi)
{
    struct wf_impl_session * session = NULL;
    if (wsi == manager->session.wsi)
    {
        session = &manager->session;
    }

    return session;
}

struct wf_impl_session * wf_impl_session_manager_get_by_inode(
    struct wf_impl_session_manager * manager,
    fuse_ino_t WF_UNUSED_PARAM(inode))
{
    // ToDo: use inode to determine session manager

    struct wf_impl_session * session = NULL;
    if (NULL != manager->session.wsi)
    {
        session = &manager->session;
    }

    return session;
}


void wf_impl_session_manager_remove(
    struct wf_impl_session_manager * manager,
    struct lws * wsi)
{
    if (wsi == manager->session.wsi)
    {
        wf_impl_session_cleanup(&manager->session);
        manager->session.wsi = NULL;
    }
}
