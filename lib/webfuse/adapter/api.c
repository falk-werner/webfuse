#include "webfuse_adapter.h"

#include "webfuse/adapter/impl/server.h"
#include "webfuse/adapter/impl/server_protocol.h"
#include "webfuse/adapter/impl/server_config.h"
#include "webfuse/adapter/impl/credentials.h"

// server

struct wf_server * wf_server_create(
    struct wf_server_config * config)
{
    return wf_impl_server_create(config);
}

void wf_server_dispose(
    struct wf_server * server)
{
    wf_impl_server_dispose(server);
}

void wf_server_run(
    struct wf_server * server)
{
    wf_impl_server_run(server);
}

void wf_server_shutdown(
    struct wf_server * server)
{
    wf_impl_server_shutdown(server);
}

// server protocol

struct wf_server_protocol * wf_server_protocol_create(
    char * mount_point)
{
    return wf_impl_server_protocol_create(mount_point);
}

void wf_server_protocol_dispose(
    struct wf_server_protocol * protocol)
{
    wf_impl_server_protocol_dispose(protocol);
}

void wf_server_protocol_init_lws(
    struct wf_server_protocol * protocol,
    struct lws_protocols * lws_protocol)
{
    wf_impl_server_protocol_init_lws(protocol, lws_protocol);
}

void wf_server_protocol_add_authenticator(
    struct wf_server_protocol * protocol,
    char const * type,
    wf_authenticate_fn * authenticate,
    void * user_data)
{
    wf_impl_server_protocol_add_authenticator(protocol, type, authenticate, user_data);
}

// server_config

struct wf_server_config * wf_server_config_create(void)
{
    return wf_impl_server_config_create();
}

void wf_server_config_dispose(
    struct wf_server_config * config)
{
    wf_impl_server_config_dispose(config);
}

void wf_server_config_set_mountpoint(
    struct wf_server_config * config,
	char const * mount_point)
{
    wf_impl_server_config_set_mountpoint(config, mount_point);
}

void wf_server_config_set_documentroot(
    struct wf_server_config * config,
	char const * document_root)
{
    wf_impl_server_config_set_documentroot(config, document_root);
}

void wf_server_config_set_keypath(
    struct wf_server_config * config,
	char const * key_path)
{
    wf_impl_server_config_set_keypath(config, key_path);
}

void wf_server_config_set_certpath(
    struct wf_server_config * config,
	char const * cert_path)
{
    wf_impl_server_config_set_certpath(config, cert_path);
}

void wf_server_config_set_vhostname(
    struct wf_server_config * config,
	char const * vhost_name)
{
    wf_impl_server_config_set_vhostname(config, vhost_name);
}

void wf_server_config_set_port(
    struct wf_server_config * config,
	int port)
{
    wf_impl_server_config_set_port(config, port);
}

void wf_server_config_add_authenticator(
    struct wf_server_config * config,
    char const * type,
    wf_authenticate_fn * authenticate,
    void * user_data)
{
    wf_impl_server_config_add_authenticator(config, type, authenticate, user_data);
}

// credentials 

char const * wf_credentials_type(
    struct wf_credentials const * credentials)
{
    return wf_impl_credentials_type(credentials);
}

char const * wf_credentials_get(
    struct wf_credentials const * credentials,
    char const * key)
{
    return wf_impl_credentials_get(credentials, key);
}
