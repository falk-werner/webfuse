#ifndef WSFS_PROVIDER_IMPL_CLIENT_CONFIG_H
#define WSFS_PROVIDER_IMPL_CLIENT_CONFIG_H

#include "wsfs/provider/client_config.h"
#include "wsfs/provider/impl/provider.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wsfsp_client_config
{
    struct wsfsp_provider provider;
    void * user_data;
    char * key_path;
    char * cert_path;
};

extern struct wsfsp_client_config * wsfsp_impl_client_config_create(void);

extern void wsfsp_impl_client_config_dispose(
    struct wsfsp_client_config * config);

extern void wsfsp_impl_client_config_set_userdata(
    struct wsfsp_client_config * config,
    void * user_data);

extern void wsfsp_impl_client_config_set_keypath(
    struct wsfsp_client_config * config,
    char const * key_path);

extern void wsfsp_impl_client_config_set_certpath(
    struct wsfsp_client_config * config,
    char const * cert_path);

extern void wsfsp_impl_client_config_set_onconnected(
    struct wsfsp_client_config * config,
    wsfsp_connected_fn * handler);

extern void wsfsp_impl_client_config_set_ondisconnected(
    struct wsfsp_client_config * config,
    wsfsp_disconnected_fn * handler);

extern void wsfsp_impl_client_config_set_ontimer(
    struct wsfsp_client_config * config,
    wsfsp_ontimer_fn * handler);

extern void wsfsp_impl_client_config_set_onlookup(
    struct wsfsp_client_config * config,
    wsfsp_lookup_fn * handler);

extern void wsfsp_impl_client_config_set_ongetattr(
    struct wsfsp_client_config * config,
    wsfsp_getattr_fn * handler);

extern void wsfsp_impl_client_config_set_onreaddir(
    struct wsfsp_client_config * config,
    wsfsp_readdir_fn * handler);

extern void wsfsp_impl_client_config_set_onopen(
    struct wsfsp_client_config * config,
    wsfsp_open_fn * handler);

extern void wsfsp_impl_client_config_set_onclose(
    struct wsfsp_client_config * config,
    wsfsp_close_fn * handler);

extern void wsfsp_impl_client_config_set_onread(
    struct wsfsp_client_config * config,
    wsfsp_read_fn * handler);

#ifdef __cplusplus
}
#endif

#endif
