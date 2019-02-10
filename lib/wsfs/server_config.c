#include "wsfs/server_config.h"

#include <stdlib.h>
#include <string.h>

static char * wsfs_server_config_strdup(char const * value)
{
    char * result = NULL;
    if (NULL != value) 
    {
        result = strdup(value);
    }

    return result;
}

void wsfs_server_config_init(
    struct wsfs_server_config * config)
{
    memset(config, 0, sizeof(struct wsfs_server_config));
}

void wsfs_server_config_cleanup(
    struct wsfs_server_config * config)
{
    free(config->mount_point);
	free(config->document_root);
	free(config->key_path);
	free(config->cert_path);
	free(config->vhost_name);

    wsfs_server_config_init(config);    
}

void wsfs_server_config_clone(
	struct wsfs_server_config * config,
	struct wsfs_server_config * clone)
{
    clone->mount_point = wsfs_server_config_strdup(config->mount_point);
	clone->document_root = wsfs_server_config_strdup(config->document_root);
	clone->key_path = wsfs_server_config_strdup(config->key_path);
	clone->cert_path = wsfs_server_config_strdup(config->cert_path);
	clone->vhost_name = wsfs_server_config_strdup(config->vhost_name);
	clone->port = config->port;
}
