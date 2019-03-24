#include "wsfs/adapter/impl/server_config.h"

#include <stdlib.h>
#include <string.h>

static char * server_config_strdup(char const * value)
{
    char * result = NULL;
    if (NULL != value) 
    {
        result = strdup(value);
    }

    return result;
}

void server_config_init(
    struct wsfs_server_config * config)
{
    memset(config, 0, sizeof(struct wsfs_server_config));

    authenticators_init(&config->authenticators);
}

void server_config_cleanup(
    struct wsfs_server_config * config)
{
    authenticators_cleanup(&config->authenticators);

    free(config->mount_point);
	free(config->document_root);
	free(config->key_path);
	free(config->cert_path);
	free(config->vhost_name);

    server_config_init(config);    
}

void server_config_clone(
	struct wsfs_server_config * config,
	struct wsfs_server_config * clone)
{
    clone->mount_point = server_config_strdup(config->mount_point);
	clone->document_root = server_config_strdup(config->document_root);
	clone->key_path = server_config_strdup(config->key_path);
	clone->cert_path = server_config_strdup(config->cert_path);
	clone->vhost_name = server_config_strdup(config->vhost_name);
	clone->port = config->port;

    authenticators_clone(&config->authenticators, &clone->authenticators);
}

struct wsfs_server_config * server_config_create(void)
{
    struct wsfs_server_config * config = malloc(sizeof(struct wsfs_server_config));
    if (NULL != config)
    {
        server_config_init(config);
    }

    return config;
}

void server_config_dispose(
    struct wsfs_server_config * config)
{
    server_config_cleanup(config);
    free(config);
}

void server_config_set_mountpoint(
    struct wsfs_server_config * config,
	char const * mount_point)
{
    free(config->mount_point);
    config->mount_point = strdup(mount_point);
}

void server_config_set_documentroot(
    struct wsfs_server_config * config,
	char const * document_root)
{
    free(config->document_root);
    config->document_root = strdup(document_root);
}

void server_config_set_keypath(
    struct wsfs_server_config * config,
	char const * key_path)
{
    free(config->key_path);
    config->key_path = strdup(key_path);
}

void server_config_set_certpath(
    struct wsfs_server_config * config,
	char const * cert_path)
{
    free(config->cert_path);
    config->cert_path = strdup(cert_path);
}

void server_config_set_vhostname(
    struct wsfs_server_config * config,
	char const * vhost_name)
{
    free(config->vhost_name);
    config->vhost_name = strdup(vhost_name);
}

void server_config_set_port(
    struct wsfs_server_config * config,
	int port)
{
    config->port = port;
}

void server_config_add_authenticator(
    struct wsfs_server_config * config,
    char const * type,
    wsfs_authenticate_fn * authenticate,
    void * user_data
)
{
    authenticators_add(&config->authenticators, type, authenticate, user_data);
}

