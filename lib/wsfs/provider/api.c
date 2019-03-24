#include "wsfs_provider.h"

#include "wsfs/provider/impl/request.h"
#include "wsfs/provider/impl/operation/getattr.h"
#include "wsfs/provider/impl/operation/lookup.h"
#include "wsfs/provider/impl/operation/readdir.h"
#include "wsfs/provider/impl/operation/open.h"
#include "wsfs/provider/impl/operation/close.h"
#include "wsfs/provider/impl/operation/read.h"
#include "wsfs/provider/impl/client_protocol.h"
#include "wsfs/provider/impl/client_config.h"
#include "wsfs/provider/impl/client.h"
#include "wsfs/provider/impl/dirbuffer.h"

// respond

void wsfsp_respond_error(
    struct wsfsp_request * request,
    wsfs_status status)
{
    wsfsp_impl_respond_error(request, status);
}

void wsfsp_respond_getattr(
    struct wsfsp_request * request,
    struct stat const * stat)
{
    wsfsp_impl_respond_getattr(request, stat);
}

void wsfsp_respond_lookup(
    struct wsfsp_request * request,
    struct stat const * stat)
{
    wsfsp_impl_respond_lookup(request, stat);
}

void wsfsp_respond_open(
    struct wsfsp_request * request,
    uint32_t handle)
{
    wsfsp_impl_respond_open(request, handle);
}

void wsfsp_respond_read(
    struct wsfsp_request * request,
    char const * data,
    size_t length)
{
    wsfsp_impl_respond_read(request, data, length);
}

void wsfsp_respond_readdir(
    struct wsfsp_request * request,
    struct wsfsp_dirbuffer * dirbuffer)
{
    wsfsp_impl_respond_readdir(request, dirbuffer);
}

// config


struct wsfsp_client_config * wsfsp_client_config_create(void)
{
    return wsfsp_impl_client_config_create();
}

void wsfsp_client_config_dispose(
    struct wsfsp_client_config * config)
{
    wsfsp_impl_client_config_dispose(config);
}

void wsfsp_client_config_set_userdata(
    struct wsfsp_client_config * config,
    void * user_data)
{
    wsfsp_impl_client_config_set_userdata(config, user_data);
}

void wsfsp_client_config_set_keypath(
    struct wsfsp_client_config * config,
    char const * key_path)
{
    wsfsp_impl_client_config_set_keypath(config, key_path);
}

void wsfsp_client_config_set_certpath(
    struct wsfsp_client_config * config,
    char const * cert_path)
{
    wsfsp_impl_client_config_set_certpath(config, cert_path);
}

void wsfsp_client_config_set_onconnected(
    struct wsfsp_client_config * config,
    wsfsp_connected_fn * handler)
{
    wsfsp_impl_client_config_set_onconnected(config, handler);
}

void wsfsp_client_config_set_ondisconnected(
    struct wsfsp_client_config * config,
    wsfsp_disconnected_fn * handler)
{
    wsfsp_impl_client_config_set_ondisconnected(config, handler);
}

void wsfsp_client_config_set_ontimer(
    struct wsfsp_client_config * config,
    wsfsp_ontimer_fn * handler)
{
    wsfsp_impl_client_config_set_ontimer(config, handler);
}

void wsfsp_client_config_set_onlookup(
    struct wsfsp_client_config * config,
    wsfsp_lookup_fn * handler)
{
    wsfsp_impl_client_config_set_onlookup(config, handler);
}

void wsfsp_client_config_set_ongetattr(
    struct wsfsp_client_config * config,
    wsfsp_getattr_fn * handler)
{
    wsfsp_impl_client_config_set_ongetattr(config, handler);
}

void wsfsp_client_config_set_onreaddir(
    struct wsfsp_client_config * config,
    wsfsp_readdir_fn * handler)
{
    wsfsp_impl_client_config_set_onreaddir(config, handler);
}

void wsfsp_client_config_set_onopen(
    struct wsfsp_client_config * config,
    wsfsp_open_fn * handler)
{
    wsfsp_impl_client_config_set_onopen(config, handler);
}

void wsfsp_client_config_set_onclose(
    struct wsfsp_client_config * config,
    wsfsp_close_fn * handler)
{
    wsfsp_impl_client_config_set_onclose(config, handler);
}

void wsfsp_client_config_set_onread(
    struct wsfsp_client_config * config,
    wsfsp_read_fn * handler)
{
    wsfsp_impl_client_config_set_onread(config, handler);
}

// protocol


struct wsfsp_client_protocol * wsfsp_client_protocol_create(
    struct wsfsp_provider const * provider,
    void * user_data)
{
    return wsfsp_impl_client_protocol_create(provider, user_data);
}

void wsfsp_client_protocol_dispose(
    struct wsfsp_client_protocol * protocol)
{
    wsfsp_impl_client_protocol_dispose(protocol);
}

void wsfsp_client_protocol_init_lws(
    struct wsfsp_client_protocol * protocol,
    struct lws_protocols * lws_protocol)
{
    wsfsp_impl_client_protocol_init_lws(protocol, lws_protocol);
}

// client

struct wsfsp_client * wsfsp_client_create(
    struct wsfsp_client_config * config)
{
    return wsfsp_impl_client_create(config);
}

void wsfsp_client_connect(
    struct wsfsp_client * client,
    char const * url)
{
    wsfsp_impl_client_connect(client, url);
}

void wsfsp_client_disconnect(
    struct wsfsp_client * client)
{
    wsfsp_impl_client_disconnect(client);
}

void wsfsp_client_dispose(
    struct wsfsp_client * client)
{
    wsfsp_impl_client_dispose(client);
}

void wsfsp_client_run(
    struct wsfsp_client * client)
{
    wsfsp_impl_client_run(client);
}

void wsfsp_client_shutdown(
    struct wsfsp_client * client)
{
    wsfsp_impl_client_shutdown(client);
}

// dirbuffer

struct wsfsp_dirbuffer * wsfsp_dirbuffer_create(void)
{
    return wsfsp_impl_dirbuffer_create();
}

void wsfsp_dirbuffer_dispose(
    struct wsfsp_dirbuffer * buffer)
{
    wsfsp_impl_dirbuffer_dispose(buffer);
}

void wsfsp_dirbuffer_add(
    struct wsfsp_dirbuffer * buffer,
    char const * name,
    ino_t inode)
{
    wsfsp_impl_dirbuffer_add(buffer, name, inode);
}

