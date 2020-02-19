#include "webfuse/provider/impl/client.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <libwebsockets.h>

#include "webfuse/provider/impl/provider.h"
#include "webfuse/provider/impl/client_protocol.h"
#include "webfuse/provider/impl/client_config.h"
#include "webfuse/core/lws_log.h"

#define WFP_CLIENT_PROTOCOL_COUNT 2

struct wfp_client
{
    struct wfp_client_protocol protocol;
    struct lws_context_creation_info info;
    struct lws_protocols protocols[WFP_CLIENT_PROTOCOL_COUNT];
    struct lws_context * context;
    char * key_path;
    char * cert_path;
};


struct wfp_client * wfp_impl_client_create(
    struct wfp_client_config * config)
{
	wf_lwslog_disable();
   
    struct wfp_client * client = malloc(sizeof(struct wfp_client));
    if (NULL != client)
    {
        wfp_impl_client_protocol_init(&client->protocol, &config->provider, config->user_data);

        memset(client->protocols, 0, sizeof(struct lws_protocols) * WFP_CLIENT_PROTOCOL_COUNT);
        client->protocols[0].name = WFP_CLIENT_PROTOCOL_NAME;
        wfp_impl_client_protocol_init_lws(&client->protocol, &client->protocols[0]);

        memset(&client->info, 0, sizeof(struct lws_context_creation_info));
        client->info.port = CONTEXT_PORT_NO_LISTEN;
        client->info.protocols = client->protocols;
        client->info.uid = -1;
        client->info.gid = -1;

        if ((NULL != config->cert_path) && (NULL != config->key_path))
        {
            
        }

        client->context = lws_create_context(&client->info);
    }

    return client;
}

void wfp_impl_client_dispose(
    struct wfp_client * client)
{
    lws_context_destroy(client->context);
    wfp_impl_client_protocol_cleanup(&client->protocol);    
    free(client);
}

void wfp_impl_client_connect(
    struct wfp_client * client,
    char const * url)
{
    wfp_impl_client_protocol_connect(&client->protocol, client->context, url);
}

void wfp_impl_client_disconnect(
    struct wfp_client * client)
{
    (void) client;

    // ToDo: implement me
}

bool wfp_impl_client_is_connected(
    struct wfp_client * client)
{
    return client->protocol.is_connected;
}

void wfp_impl_client_service(
    struct wfp_client * client,
    int timeout_ms)
{
    lws_service(client->context, timeout_ms);
}

