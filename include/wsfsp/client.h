#ifndef _WSFSP_CLIENT_H
#define _WSFSP_CLIENT_H

#include "wsfs/api.h"

struct wsfs_provider;
struct wsfs_provider_client_config;

#ifdef __cplusplus
extern "C"
{
#endif

extern WSFS_API struct wsfs_provider_client * wsfs_provider_client_create(
    struct wsfs_provider * provider,
    void * user_data);

extern WSFS_API void wsfs_provider_client_connect(
    struct wsfs_provider_client * client,
    char const * url);

extern WSFS_API void wsfs_provider_client_disconnect(
    struct wsfs_provider_client * client);

extern WSFS_API void wsfs_provider_client_settimeout(
    struct wsfs_provider_client * client,
    unsigned int timepoint);

extern WSFS_API void wsfs_provider_client_dispose(
    struct wsfs_provider_client * client);

extern WSFS_API void wsfs_provider_client_run(
    struct wsfs_provider_client * client);

extern WSFS_API void wsfs_provider_client_shutdown(
    struct wsfs_provider_client * client);


#ifdef __cplusplus
}
#endif


#endif
