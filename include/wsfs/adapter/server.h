#ifndef WSFS_ADAPTER_SERVER_H
#define WSFS_ADAPTER_SERVER_H

#include "wsfs/adapter/api.h"

struct wsfs_server;
struct wsfs_server_config;

#ifdef __cplusplus
extern "C"
{
#endif

extern WSFSA_API struct wsfs_server * wsfs_server_create(
    struct wsfs_server_config * config);

extern WSFSA_API void wsfs_server_dispose(
    struct wsfs_server * server);

extern WSFSA_API void wsfs_server_run(
    struct wsfs_server * server);

extern WSFSA_API void wsfs_server_shutdown(
    struct wsfs_server * server);

#ifdef __cplusplus
}
#endif


#endif
