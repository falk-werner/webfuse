#include "webfuse/adapter/impl/server_config.h"

#include <stdlib.h>
#include <string.h>

static char * wf_impl_server_config_strdup(char const * value)
{
    char * result = NULL;
    if (NULL != value) 
    {
        result = strdup(value);
    }

    return result;
}

void wf_impl_server_config_init(
    struct wf_server_config * config)
{
    memset(config, 0, sizeof(struct wf_server_config));

    wf_impl_authenticators_init(&config->authenticators);
}

void wf_impl_server_config_cleanup(
    struct wf_server_config * config)
{
    wf_impl_authenticators_cleanup(&config->authenticators);

    free(config->mount_point);
	free(config->document_root);
	free(config->key_path);
	free(config->cert_path);
	free(config->vhost_name);

    wf_impl_server_config_init(config);    
}

void wf_impl_server_config_clone(
	struct wf_server_config * config,
	struct wf_server_config * clone)
{
    clone->mount_point = wf_impl_server_config_strdup(config->mount_point);
	clone->document_root = wf_impl_server_config_strdup(config->document_root);
	clone->key_path = wf_impl_server_config_strdup(config->key_path);
	clone->cert_path = wf_impl_server_config_strdup(config->cert_path);
	clone->vhost_name = wf_impl_server_config_strdup(config->vhost_name);
	clone->port = config->port;

    wf_impl_authenticators_clone(&config->authenticators, &clone->authenticators);
}

struct wf_server_config * wf_impl_server_config_create(void)
{
    struct wf_server_config * config = malloc(sizeof(struct wf_server_config));
    if (NULL != config)
    {
        wf_impl_server_config_init(config);
    }

    return config;
}

void wf_impl_server_config_dispose(
    struct wf_server_config * config)
{
    wf_impl_server_config_cleanup(config);
    free(config);
}

void wf_impl_server_config_set_mountpoint(
    struct wf_server_config * config,
	char const * mount_point)
{
    free(config->mount_point);
    config->mount_point = strdup(mount_point);
}

void wf_impl_server_config_set_documentroot(
    struct wf_server_config * config,
	char const * document_root)
{
    free(config->document_root);
    config->document_root = strdup(document_root);
}

void wf_impl_server_config_set_keypath(
    struct wf_server_config * config,
	char const * key_path)
{
    free(config->key_path);
    config->key_path = strdup(key_path);
}

void wf_impl_server_config_set_certpath(
    struct wf_server_config * config,
	char const * cert_path)
{
    free(config->cert_path);
    config->cert_path = strdup(cert_path);
}

void wf_impl_server_config_set_vhostname(
    struct wf_server_config * config,
	char const * vhost_name)
{
    free(config->vhost_name);
    config->vhost_name = strdup(vhost_name);
}

void wf_impl_server_config_set_port(
    struct wf_server_config * config,
	int port)
{
    config->port = port;
}

void wf_impl_server_config_add_authenticator(
    struct wf_server_config * config,
    char const * type,
    wf_authenticate_fn * authenticate,
    void * user_data
)
{
    wf_impl_authenticators_add(&config->authenticators, type, authenticate, user_data);
}

