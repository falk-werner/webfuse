#ifndef WSFS_PROVIDER_CLIENT_H
#define WSFS_PROVIDER_CLIENT_H

#include "wsfs/provider/api.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wsfsp_client;
struct wsfsp_client_config;

extern WSFSP_API struct wsfsp_client * wsfsp_client_create(
    struct wsfsp_client_config * config);

extern WSFSP_API void wsfsp_client_connect(
    struct wsfsp_client * client,
    char const * url);

extern WSFSP_API void wsfsp_client_disconnect(
    struct wsfsp_client * client);

extern WSFSP_API void wsfsp_client_dispose(
    struct wsfsp_client * client);

extern WSFSP_API void wsfsp_client_run(
    struct wsfsp_client * client);

extern WSFSP_API void wsfsp_client_shutdown(
    struct wsfsp_client * client);


#ifdef __cplusplus
}
#endif


#endif
