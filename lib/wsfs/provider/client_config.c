#include "wsfs/provider/client_config_intern.h"
#include "wsfs/provider/provider.h"
#include <stdlib.h>
#include <string.h>

struct wsfsp_client_config * wsfsp_client_config_create(void)
{
    struct wsfsp_client_config * config = malloc(sizeof(struct wsfsp_client_config));
    if (NULL != config)
    {
        wsfsp_provider_init(&config->provider);
        config->user_data = NULL;
        config->key_path = NULL;
        config->cert_path = NULL;
    }

    return config;
}

void wsfsp_client_config_dispose(
    struct wsfsp_client_config * config)
{
    free(config->key_path);
    free(config->cert_path);
    free(config);
}

void wsfsp_client_config_set_userdata(
    struct wsfsp_client_config * config,
    void * user_data)
{
    config->user_data = user_data;
}

void wsfsp_client_config_set_keypath(
    struct wsfsp_client_config * config,
    char const * key_path)
{
    free(config->key_path);
    config->key_path = strdup(key_path);
}

void wsfsp_client_config_set_certpath(
    struct wsfsp_client_config * config,
    char const * cert_path)
{
    free(config->cert_path);
    config->cert_path = strdup(cert_path);
}

void wsfsp_client_config_set_onconnected(
    struct wsfsp_client_config * config,
    wsfsp_connected_fn * handler)
{
    config->provider.connected = handler;
}

void wsfsp_client_config_set_ondisconnected(
    struct wsfsp_client_config * config,
    wsfsp_disconnected_fn * handler)
{
    config->provider.disconnected = handler;
}

void wsfsp_client_config_set_ontimer(
    struct wsfsp_client_config * config,
    wsfsp_ontimer_fn * handler)
{
    config->provider.ontimer = handler;
}

void wsfsp_client_config_set_onlookup(
    struct wsfsp_client_config * config,
    wsfsp_lookup_fn * handler)
{
    config->provider.lookup = handler;
}

void wsfsp_client_config_set_ongetattr(
    struct wsfsp_client_config * config,
    wsfsp_getattr_fn * handler)
{
    config->provider.getattr = handler;
}

void wsfsp_client_config_set_onreaddir(
    struct wsfsp_client_config * config,
    wsfsp_readdir_fn * handler)
{
    config->provider.readdir = handler;
}

void wsfsp_client_config_set_onopen(
    struct wsfsp_client_config * config,
    wsfsp_open_fn * handler)
{
    config->provider.open = handler;
}

void wsfsp_client_config_set_onclose(
    struct wsfsp_client_config * config,
    wsfsp_close_fn * handler)
{
    config->provider.close = handler;
}

void wsfsp_client_config_set_onread(
    struct wsfsp_client_config * config,
    wsfsp_read_fn * handler)
{
    config->provider.read = handler;
}
