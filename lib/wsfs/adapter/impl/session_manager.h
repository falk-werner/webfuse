#ifndef WSFS_ADAPTER_IMPL_SESSION_MANAGER_H
#define WSFS_ADAPTER_IMPL_SESSION_MANAGER_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#include "wsfs/adapter/impl/session.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct lws;

struct session_manager
{
    struct session session;
};

extern void session_manager_init(
    struct session_manager * manager);

extern void session_manager_cleanup(
    struct session_manager * manager);

extern struct session * session_manager_add(
    struct session_manager * manager,
    struct lws * wsi,
    struct authenticators * authenticators,
    struct jsonrpc_server * rpc);

extern struct session * session_manager_get(
    struct session_manager * manager,
    struct lws * wsi);

extern void session_manager_remove(
    struct session_manager * manager,
    struct lws * wsi);

#ifdef __cplusplus
}
#endif

#endif