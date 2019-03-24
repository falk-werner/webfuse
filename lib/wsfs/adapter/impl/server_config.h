#ifndef WSFS_ADAPTER_IMPL_SERVER_CONFIG_H
#define WSFS_ADAPTER_IMPL_SERVER_CONFIG_H

#include "wsfs/adapter/impl/authenticators.h"

#ifdef __cplusplus
extern "C" {
#endif

struct server_config
{
    char * mount_point;
	char * document_root;
	char * key_path;
	char * cert_path;
	char * vhost_name;
	int port;
	struct authenticators authenticators;
};

extern struct server_config * server_config_create(void);

extern void server_config_dispose(
    struct server_config * config);

extern void server_config_init(
    struct server_config * config);

extern void server_config_cleanup(
    struct server_config * config);

extern void server_config_clone(
	struct server_config * config,
	struct server_config * clone);

extern void server_config_set_mountpoint(
    struct server_config * config,
	char const * mount_point);

extern void server_config_set_documentroot(
    struct server_config * config,
	char const * document_root);

extern void server_config_set_keypath(
    struct server_config * config,
	char const * key_path);

extern void server_config_set_certpath(
    struct server_config * config,
	char const * cert_path);

extern void server_config_set_vhostname(
    struct server_config * config,
	char const * vhost_name);

extern void server_config_set_port(
    struct server_config * config,
	int port);

extern void server_config_add_authenticator(
    struct server_config * config,
    char const * type,
    authenticate_fn * authenticate,
    void * user_data
);


#ifdef __cplusplus
}
#endif

#endif
