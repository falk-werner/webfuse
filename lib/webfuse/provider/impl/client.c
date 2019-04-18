#include "webfuse/provider/impl/client.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <libwebsockets.h>

#include "webfuse/provider/impl/provider.h"
#include "webfuse/provider/impl/client_protocol.h"
#include "webfuse/provider/impl/client_config.h"
#include "webfuse/provider/impl/url.h"

#define WFP_PROTOCOL ("fs")
#define WFP_DISABLE_LWS_LOG 0
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
 	lws_set_log_level(WFP_DISABLE_LWS_LOG, NULL);
   
    struct wfp_client * client = malloc(sizeof(struct wfp_client));
    if (NULL != client)
    {
        wfp_impl_client_protocol_init(&client->protocol, &config->provider, config->user_data);

        memset(client->protocols, 0, sizeof(struct lws_protocols) * WFP_CLIENT_PROTOCOL_COUNT);
        client->protocols[0].name = "fs";
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
    struct wfp_impl_url url_data;
    bool const success = wfp_impl_url_init(&url_data, url);
    if (success)
    {
        struct lws_client_connect_info info;
        memset(&info, 0, sizeof(struct lws_client_connect_info));
        info.context = client->context;
        info.port = url_data.port;
        info.address = url_data.host;
        info.path = url_data.path;
        info.host = info.address;
        info.origin = info.address;
        info.ssl_connection = (url_data.use_tls) ? LCCSCF_USE_SSL : 0;
        info.protocol = WFP_PROTOCOL;
        info.pwsi = &client->protocol.wsi;

        lws_client_connect_via_info(&info);

        wfp_impl_url_cleanup(&url_data);
    }
}

void wfp_impl_client_disconnect(
    struct wfp_client * client)
{
    (void) client;

    // ToDo: implement me
}

void wfp_impl_client_service(
    struct wfp_client * client,
    int timeout_ms)
{
    lws_service(client->context, timeout_ms);
}

