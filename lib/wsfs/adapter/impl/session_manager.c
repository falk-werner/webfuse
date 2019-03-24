#include "wsfs/adapter/impl/session_manager.h"
#include <stddef.h>

void wsfs_impl_session_manager_init(
    struct wsfs_impl_session_manager * manager)
{
    wsfs_impl_session_init(&manager->session, NULL, NULL, NULL);
}

void wsfs_impl_session_manager_cleanup(
    struct wsfs_impl_session_manager * manager)
{
    wsfs_impl_session_cleanup(&manager->session);
}

struct wsfs_impl_session * wsfs_impl_session_manager_add(
    struct wsfs_impl_session_manager * manager,
    struct lws * wsi,
    struct wsfs_impl_authenticators * authenticators,
    struct wsfs_impl_jsonrpc_server * rpc)
{
    struct wsfs_impl_session * session = NULL; 
    if (NULL == manager->session.wsi)
    {
        session = &manager->session;
        wsfs_impl_session_init(&manager->session, wsi, authenticators, rpc);        
    }

    return session;
}

struct wsfs_impl_session * wsfs_impl_session_manager_get(
    struct wsfs_impl_session_manager * manager,
    struct lws * wsi)
{
    struct wsfs_impl_session * session = NULL;
    if (wsi == manager->session.wsi)
    {
        session = &manager->session;
    }

    return session;
}

void wsfs_impl_session_manager_remove(
    struct wsfs_impl_session_manager * manager,
    struct lws * wsi)
{
    if (wsi == manager->session.wsi)
    {
        wsfs_impl_session_cleanup(&manager->session);
        manager->session.wsi = NULL;
    }
}
