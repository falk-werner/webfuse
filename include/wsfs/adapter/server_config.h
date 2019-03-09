#ifndef WSFS_ADAPTER_SERVER_CONFIG_H
#define WSFS_ADAPTER_SERVER_CONFIG_H

#include "wsfs/adapter/api.h"

struct wsfs_server_config;

#ifdef __cplusplus
extern "C"
{
#endif

extern WSFSA_API struct wsfs_server_config * wsfs_server_config_create(void);

extern WSFSA_API void wsfs_server_config_dispose(
    struct wsfs_server_config * config);


extern WSFSA_API void wsfs_server_config_set_mountpoint(
    struct wsfs_server_config * config,
	char const * mount_point);

extern WSFSA_API void wsfs_server_config_set_documentroot(
    struct wsfs_server_config * config,
	char const * document_root);

extern WSFSA_API void wsfs_server_config_set_keypath(
    struct wsfs_server_config * config,
	char const * key_path);

extern WSFSA_API void wsfs_server_config_set_certpath(
    struct wsfs_server_config * config,
	char const * cert_path);

extern WSFSA_API void wsfs_server_config_set_vhostname(
    struct wsfs_server_config * config,
	char const * vhost_name);

extern WSFSA_API void wsfs_server_config_set_port(
    struct wsfs_server_config * config,
	int port);

#ifdef __cplusplus
}
#endif

#endif
