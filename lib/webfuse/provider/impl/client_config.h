#ifndef WF_PROVIDER_IMPL_CLIENT_CONFIG_H
#define WF_PROVIDER_IMPL_CLIENT_CONFIG_H

#include "webfuse/provider/client_config.h"
#include "webfuse/provider/impl/provider.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wfp_client_config
{
    struct wfp_provider provider;
    void * user_data;
    char * key_path;
    char * cert_path;
    char * ca_filepath;
};

extern struct wfp_client_config * wfp_impl_client_config_create(void);

extern void wfp_impl_client_config_dispose(
    struct wfp_client_config * config);

extern void wfp_impl_client_config_set_userdata(
    struct wfp_client_config * config,
    void * user_data);

extern void wfp_impl_client_config_set_keypath(
    struct wfp_client_config * config,
    char const * key_path);

extern void wfp_impl_client_config_set_certpath(
    struct wfp_client_config * config,
    char const * cert_path);

extern void wfp_impl_client_config_set_ca_filepath(
    struct wfp_client_config * config,
    char const * ca_filepath);

extern void wfp_impl_client_config_set_onconnected(
    struct wfp_client_config * config,
    wfp_connected_fn * handler);

extern void wfp_impl_client_config_set_ondisconnected(
    struct wfp_client_config * config,
    wfp_disconnected_fn * handler);

extern void wfp_impl_client_config_set_onlookup(
    struct wfp_client_config * config,
    wfp_lookup_fn * handler);

extern void wfp_impl_client_config_set_ongetattr(
    struct wfp_client_config * config,
    wfp_getattr_fn * handler);

extern void wfp_impl_client_config_set_onreaddir(
    struct wfp_client_config * config,
    wfp_readdir_fn * handler);

extern void wfp_impl_client_config_set_onopen(
    struct wfp_client_config * config,
    wfp_open_fn * handler);

extern void wfp_impl_client_config_set_onclose(
    struct wfp_client_config * config,
    wfp_close_fn * handler);

extern void wfp_impl_client_config_set_onread(
    struct wfp_client_config * config,
    wfp_read_fn * handler);

extern void wfp_impl_client_config_enable_authentication(
    struct wfp_client_config * config,
    wfp_get_credentials_fn * get_credentials);

#ifdef __cplusplus
}
#endif

#endif
