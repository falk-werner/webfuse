#include "wsfs_adapter.h"

#include "wsfs/adapter/impl/server.h"
#include "wsfs/adapter/impl/server_protocol.h"
#include "wsfs/adapter/impl/server_config.h"
#include "wsfs/adapter/impl/credentials.h"

// server

struct wsfs_server * wsfs_server_create(
    struct wsfs_server_config * config)
{
    return server_create(config);
}

void wsfs_server_dispose(
    struct wsfs_server * server)
{
    server_dispose(server);
}

void wsfs_server_run(
    struct wsfs_server * server)
{
    server_run(server);
}

void wsfs_server_shutdown(
    struct wsfs_server * server)
{
    server_shutdown(server);
}

// server protocol

struct wsfs_server_protocol * wsfs_server_protocol_create(
    char * mount_point)
{
    return server_protocol_create(mount_point);
}

void wsfs_server_protocol_dispose(
    struct wsfs_server_protocol * protocol)
{
    server_protocol_dispose(protocol);
}

void wsfs_server_protocol_init_lws(
    struct wsfs_server_protocol * protocol,
    struct lws_protocols * lws_protocol)
{
    server_protocol_init_lws(protocol, lws_protocol);
}

void wsfs_server_protocol_add_authenticator(
    struct wsfs_server_protocol * protocol,
    char const * type,
    wsfs_authenticate_fn * authenticate,
    void * user_data)
{
    server_protocol_add_authenticator(protocol, type, authenticate, user_data);
}

// server_config

struct wsfs_server_config * wsfs_server_config_create(void)
{
    return server_config_create();
}

void wsfs_server_config_dispose(
    struct wsfs_server_config * config)
{
    server_config_dispose(config);
}

void wsfs_server_config_set_mountpoint(
    struct wsfs_server_config * config,
	char const * mount_point)
{
    server_config_set_mountpoint(config, mount_point);
}

void wsfs_server_config_set_documentroot(
    struct wsfs_server_config * config,
	char const * document_root)
{
    server_config_set_documentroot(config, document_root);
}

void wsfs_server_config_set_keypath(
    struct wsfs_server_config * config,
	char const * key_path)
{
    server_config_set_keypath(config, key_path);
}

void wsfs_server_config_set_certpath(
    struct wsfs_server_config * config,
	char const * cert_path)
{
    server_config_set_certpath(config, cert_path);
}

void wsfs_server_config_set_vhostname(
    struct wsfs_server_config * config,
	char const * vhost_name)
{
    server_config_set_vhostname(config, vhost_name);
}

void wsfs_server_config_set_port(
    struct wsfs_server_config * config,
	int port)
{
    server_config_set_port(config, port);
}

void wsfs_server_config_add_authenticator(
    struct wsfs_server_config * config,
    char const * type,
    wsfs_authenticate_fn * authenticate,
    void * user_data)
{
    server_config_add_authenticator(config, type, authenticate, user_data);
}

// credentials 

char const * wsfs_credentials_type(
    struct wsfs_credentials const * credentials)
{
    return credentials_type(credentials);
}

char const * wsfs_credentials_get(
    struct wsfs_credentials const * credentials,
    char const * key)
{
    return credentials_get(credentials, key);
}
