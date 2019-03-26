#ifndef WSFS_PROVIDER_IMPL_CLIENT_H
#define WSFS_PROVIDER_IMPL_CLIENT_H

#ifdef __cplusplus
extern "C"
{
#endif

struct wsfsp_client;
struct wsfsp_client_config;

extern struct wsfsp_client * wsfsp_impl_client_create(
    struct wsfsp_client_config * config);

extern void wsfsp_impl_client_set_keypath(
    struct wsfsp_client * client,
    char * key_path);

extern void wsfsp_impl_client_set_certpath(
    struct wsfsp_client * client,
    char * cert_path);

extern void wsfsp_impl_client_connect(
    struct wsfsp_client * client,
    char const * url);

extern void wsfsp_impl_client_disconnect(
    struct wsfsp_client * client);

extern void wsfsp_impl_client_settimeout(
    struct wsfsp_client * client,
    unsigned int timepoint);

extern void wsfsp_impl_client_dispose(
    struct wsfsp_client * client);

extern void wsfsp_impl_client_run(
    struct wsfsp_client * client);

extern void wsfsp_impl_client_shutdown(
    struct wsfsp_client * client);


#ifdef __cplusplus
}
#endif


#endif
