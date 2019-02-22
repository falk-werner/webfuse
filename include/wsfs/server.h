#ifndef WSFS_SERVER_H
#define WSFS_SERVER_H

#include "wsfs/api.h"

struct wsfs_server;
struct wsfs_server_config;

#ifdef __cplusplus
extern "C"
{
#endif

extern WSFS_API struct wsfs_server * wsfs_server_create(
    struct wsfs_server_config * config);

extern WSFS_API void wsfs_server_dispose(
    struct wsfs_server * server);

extern WSFS_API void wsfs_server_run(
    struct wsfs_server * server);

extern WSFS_API void wsfs_server_shutdown(
    struct wsfs_server * server);

#ifdef __cplusplus
}
#endif


#endif
