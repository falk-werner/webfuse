#ifndef WF_PROVIDER_IMPL_CLIENT_H
#define WF_PROVIDER_IMPL_CLIENT_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

struct wfp_client;
struct wfp_client_config;

extern struct wfp_client * wfp_impl_client_create(
    struct wfp_client_config * config);

extern void wfp_impl_client_set_keypath(
    struct wfp_client * client,
    char * key_path);

extern void wfp_impl_client_set_certpath(
    struct wfp_client * client,
    char * cert_path);

extern void wfp_impl_client_connect(
    struct wfp_client * client,
    char const * url);

extern void wfp_impl_client_disconnect(
    struct wfp_client * client);

extern void wfp_impl_client_dispose(
    struct wfp_client * client);

extern void wfp_impl_client_service(
    struct wfp_client * client);

extern void wfp_impl_client_interrupt(
    struct wfp_client * client);

#ifdef __cplusplus
}
#endif


#endif
