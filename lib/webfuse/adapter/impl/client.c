#include "webfuse/adapter/impl/client.h"
#include "webfuse/adapter/impl/client_protocol.h"
#include "webfuse/adapter/impl/client_tlsconfig.h"
#include "webfuse/core/lws_log.h"

#include <libwebsockets.h>

#include <stdlib.h>
#include <string.h>

#define WF_CLIENT_PROTOCOL_COUNT 2

struct wf_client
{
    struct wf_client_protocol protocol;
    struct lws_context_creation_info info;
    struct lws_protocols protocols[WF_CLIENT_PROTOCOL_COUNT];
    struct wf_client_tlsconfig tls;
    struct lws_context * context;
    void * user_data;
};

struct wf_client *
wf_impl_client_create(
    wf_client_callback_fn * callback,
    void * user_data)
{
    wf_lwslog_disable();

    struct wf_client * client = malloc(sizeof(struct wf_client));
    wf_impl_client_tlsconfig_init(&client->tls);
    client->user_data = user_data;
    wf_impl_client_protocol_init(&client->protocol, 
        (wf_client_protocol_callback_fn*) callback, (void*) client);

    memset(client->protocols, 0, sizeof(struct lws_protocols) * WF_CLIENT_PROTOCOL_COUNT);
    wf_impl_client_protocol_init_lws(&client->protocol, &client->protocols[0]);

    memset(&client->info, 0, sizeof(struct lws_context_creation_info));
    client->info.port = CONTEXT_PORT_NO_LISTEN;
    client->info.protocols = client->protocols;
    client->info.uid = -1;
    client->info.gid = -1;

    wf_impl_client_protocol_callback(&client->protocol, WF_CLIENT_GET_TLS_CONFIG, &client->tls);
    if (wf_impl_client_tlsconfig_isset(&client->tls))
    {
        client->info.options |= LWS_SERVER_OPTION_EXPLICIT_VHOSTS;
    }

    client->context = lws_create_context(&client->info);

    if (wf_impl_client_tlsconfig_isset(&client->tls))
    {
        struct lws_vhost * vhost = lws_create_vhost(client->context, &client->info);
		client->info.options |= LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
        client->info.client_ssl_cert_filepath = client->tls.cert_path;
        client->info.client_ssl_private_key_filepath = client->tls.key_path;
        client->info.client_ssl_ca_filepath = client->tls.cafile_path;
        lws_init_vhost_client_ssl(&client->info, vhost);
    }

    wf_impl_client_protocol_callback(&client->protocol, WF_CLIENT_CREATED ,NULL);
    return client;
}

void
wf_impl_client_dispose(
    struct wf_client * client)
{
    lws_context_destroy(client->context);
    wf_impl_client_protocol_cleanup(&client->protocol);
    wf_impl_client_tlsconfig_cleanup(&client->tls);
    free(client);
}

void *
wf_impl_client_get_userdata(
    struct wf_client * client)
{
    return client->user_data;
}

void
wf_impl_client_service(
    struct wf_client * client)
{
    lws_service(client->context, 0);
}

void
wf_impl_client_interrupt(
    struct wf_client * client)
{
    lws_cancel_service(client->context);
}

void
wf_impl_client_connect(
    struct wf_client * client,
    char const * url)
{
    wf_impl_client_protocol_connect(&client->protocol, client->context, url);
}

void
wf_impl_client_disconnect(
    struct wf_client * client)
{
    wf_impl_client_protocol_disconnect(&client->protocol);
}

void
wf_impl_client_authenticate(
    struct wf_client * client)
{
    wf_impl_client_protocol_authenticate(&client->protocol);
}

void
wf_impl_client_add_filesystem(
    struct wf_client * client,
    char const * local_path,
    char const * name)
{
    wf_impl_client_protocol_add_filesystem(&client->protocol, local_path, name);
}
