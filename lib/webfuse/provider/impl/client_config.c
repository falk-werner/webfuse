#include "webfuse/provider/impl/client_config.h"
#include "webfuse/provider/impl/provider.h"
#include <stdlib.h>
#include <string.h>

struct wfp_client_config * wfp_impl_client_config_create(void)
{
    struct wfp_client_config * config = malloc(sizeof(struct wfp_client_config));
    wfp_impl_provider_init(&config->provider);
    config->user_data = NULL;
    config->key_path = NULL;
    config->cert_path = NULL;
    config->ca_filepath = NULL;

    return config;
}

void wfp_impl_client_config_dispose(
    struct wfp_client_config * config)
{
    free(config->key_path);
    free(config->cert_path);
    free(config->ca_filepath);
    free(config);
}

void wfp_impl_client_config_set_userdata(
    struct wfp_client_config * config,
    void * user_data)
{
    config->user_data = user_data;
}

void wfp_impl_client_config_set_keypath(
    struct wfp_client_config * config,
    char const * key_path)
{
    free(config->key_path);
    config->key_path = strdup(key_path);
}

void wfp_impl_client_config_set_certpath(
    struct wfp_client_config * config,
    char const * cert_path)
{
    free(config->cert_path);
    config->cert_path = strdup(cert_path);
}

void wfp_impl_client_config_set_ca_filepath(
    struct wfp_client_config * config,
    char const * ca_filepath)
{
    free(config->ca_filepath);
    config->ca_filepath = strdup(ca_filepath); 
}

void wfp_impl_client_config_set_onconnected(
    struct wfp_client_config * config,
    wfp_connected_fn * handler)
{
    config->provider.connected = handler;
}

void wfp_impl_client_config_set_ondisconnected(
    struct wfp_client_config * config,
    wfp_disconnected_fn * handler)
{
    config->provider.disconnected = handler;
}

void wfp_impl_client_config_set_onlookup(
    struct wfp_client_config * config,
    wfp_lookup_fn * handler)
{
    config->provider.lookup = handler;
}

void wfp_impl_client_config_set_ongetattr(
    struct wfp_client_config * config,
    wfp_getattr_fn * handler)
{
    config->provider.getattr = handler;
}

void wfp_impl_client_config_set_onreaddir(
    struct wfp_client_config * config,
    wfp_readdir_fn * handler)
{
    config->provider.readdir = handler;
}

void wfp_impl_client_config_set_onopen(
    struct wfp_client_config * config,
    wfp_open_fn * handler)
{
    config->provider.open = handler;
}

void wfp_impl_client_config_set_onclose(
    struct wfp_client_config * config,
    wfp_close_fn * handler)
{
    config->provider.close = handler;
}

void wfp_impl_client_config_set_onread(
    struct wfp_client_config * config,
    wfp_read_fn * handler)
{
    config->provider.read = handler;
}

void wfp_impl_client_config_enable_authentication(
    struct wfp_client_config * config,
    wfp_get_credentials_fn * get_credentials)
{
    config->provider.get_credentials = get_credentials;
}
