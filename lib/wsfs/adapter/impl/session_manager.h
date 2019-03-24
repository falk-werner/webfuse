#ifndef WSFS_ADAPTER_SESSION_MANAGER_H
#define WSFS_ADAPTER_SESSION_MANAGER_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#include "wsfs/adapter/impl/session.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct lws;

struct wsfs_session_manager
{
    struct wsfs_session session;
};

extern void wsfs_session_manager_init(
    struct wsfs_session_manager * manager);

extern void wsfs_session_manager_cleanup(
    struct wsfs_session_manager * manager);

extern struct wsfs_session * wsfs_session_manager_add(
    struct wsfs_session_manager * manager,
    struct lws * wsi,
    struct wsfs_authenticators * authenticators,
    struct wsfs_jsonrpc_server * rpc);

extern struct wsfs_session * wsfs_session_manager_get(
    struct wsfs_session_manager * manager,
    struct lws * wsi);

extern void wsfs_session_manager_remove(
    struct wsfs_session_manager * manager,
    struct lws * wsi);

#ifdef __cplusplus
}
#endif

#endif