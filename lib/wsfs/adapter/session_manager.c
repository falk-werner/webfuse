#include "wsfs/adapter/session_manager.h"
#include <stddef.h>

void wsfs_session_manager_init(
    struct wsfs_session_manager * manager)
{
    wsfs_session_init(&manager->session, NULL, false);
}

void wsfs_session_manager_cleanup(
    struct wsfs_session_manager * manager)
{
    wsfs_session_cleanup(&manager->session);
}

bool wsfs_session_manager_add(
    struct wsfs_session_manager * manager,
    struct lws * wsi,
    bool is_authenticated)
{
    bool const result = (NULL == manager->session.wsi);
    if (result)
    {
        wsfs_session_init(&manager->session, wsi, is_authenticated);
    }

    return result;
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
