#include "wsfs/adapter/session_manager.h"
#include <stddef.h>

void wsfs_session_manager_init(
    struct wsfs_session_manager * manager)
{
    wsfs_session_init(&manager->session, NULL, NULL, NULL);
}

void wsfs_session_manager_cleanup(
    struct wsfs_session_manager * manager)
{
    wsfs_session_cleanup(&manager->session);
}

struct wsfs_session * wsfs_session_manager_add(
    struct wsfs_session_manager * manager,
    struct lws * wsi,
    struct wsfs_authenticators * authenticators,
    struct wsfs_jsonrpc_server * rpc)
{
    struct wsfs_session * session = NULL; 
    if (NULL == manager->session.wsi)
    {
        session = &manager->session;
        wsfs_session_init(&manager->session, wsi, authenticators, rpc);        
    }

    return session;
}

struct wsfs_session * wsfs_session_manager_get(
    struct wsfs_session_manager * manager,
    struct lws * wsi)
{
    struct wsfs_session * session = NULL;
    if (wsi == manager->session.wsi)
    {
        session = &manager->session;
    }

    return session;
}

void wsfs_session_manager_remove(
    struct wsfs_session_manager * manager,
    struct lws * wsi)
{
    if (wsi == manager->session.wsi)
    {
        wsfs_session_cleanup(&manager->session);
        manager->session.wsi = NULL;
    }
}
