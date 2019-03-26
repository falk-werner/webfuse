#ifndef WF_PROVIDER_IMPL_CLIENT_H
#define WF_PROVIDER_IMPL_CLIENT_H

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

extern void wfp_impl_client_settimeout(
    struct wfp_client * client,
    unsigned int timepoint);

extern void wfp_impl_client_dispose(
    struct wfp_client * client);

extern void wfp_impl_client_run(
    struct wfp_client * client);

extern void wfp_impl_client_shutdown(
    struct wfp_client * client);


#ifdef __cplusplus
}
#endif


#endif
