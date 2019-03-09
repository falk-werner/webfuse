#ifndef WSFS_ADAPTER_SERVER_CONFIG_INTERN_H
#define WSFS_ADAPTER_SERVER_CONFIG_INTERN_H

#include "wsfs/adapter/server_config.h"

struct wsfs_server_config
{
    char * mount_point;
	char * document_root;
	char * key_path;
	char * cert_path;
	char * vhost_name;
	int port;
};

#ifdef __cplusplus
extern "C"
{
#endif

extern void wsfs_server_config_init(
    struct wsfs_server_config * config);

extern void wsfs_server_config_cleanup(
    struct wsfs_server_config * config);

extern void wsfs_server_config_clone(
	struct wsfs_server_config * config,
	struct wsfs_server_config * clone);

#ifdef __cplusplus
}
#endif

#endif
