#include "wsfs/adapter/impl/session_manager.h"
#include <stddef.h>

void session_manager_init(
    struct session_manager * manager)
{
    session_init(&manager->session, NULL, NULL, NULL);
}

void session_manager_cleanup(
    struct session_manager * manager)
{
    session_cleanup(&manager->session);
}

struct session * session_manager_add(
    struct session_manager * manager,
    struct lws * wsi,
    struct authenticators * authenticators,
    struct jsonrpc_server * rpc)
{
    struct session * session = NULL; 
    if (NULL == manager->session.wsi)
    {
        session = &manager->session;
        session_init(&manager->session, wsi, authenticators, rpc);        
    }

    return session;
}

struct session * session_manager_get(
    struct session_manager * manager,
    struct lws * wsi)
{
    struct session * session = NULL;
    if (wsi == manager->session.wsi)
    {
        session = &manager->session;
    }

    return session;
}

void session_manager_remove(
    struct session_manager * manager,
    struct lws * wsi)
{
    if (wsi == manager->session.wsi)
    {
        session_cleanup(&manager->session);
        manager->session.wsi = NULL;
    }
}
