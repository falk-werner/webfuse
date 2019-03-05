#ifndef WSFS_PROVIDER_CLIENT_H
#define WSFS_PROVIDER_CLIENT_H

#include "wsfs/provider/api.h"

struct wsfsp_client;
struct wsfsp_client_config;

#ifdef __cplusplus
extern "C"
{
#endif

extern WSFSP_API struct wsfsp_client * wsfsp_client_create(
    struct wsfsp_client_config * config);

extern WSFSP_API void wsfsp_client_set_keypath(
    struct wsfsp_client * client,
    char * key_path);

extern WSFSP_API void wsfsp_client_set_certpath(
    struct wsfsp_client * client,
    char * cert_path);

extern WSFSP_API void wsfsp_client_connect(
    struct wsfsp_client * client,
    char const * url);

extern WSFSP_API void wsfsp_client_disconnect(
    struct wsfsp_client * client);

extern WSFSP_API void wsfsp_client_settimeout(
    struct wsfsp_client * client,
    unsigned int timepoint);

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
