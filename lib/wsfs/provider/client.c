#include "wsfs/provider/client.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <libwebsockets.h>

#include "wsfs/provider/provider.h"
#include "wsfs/provider/client_protocol_intern.h"
#include "wsfs/provider/url.h"

#define WSFSP_PROTOCOL ("fs")
#define WSFSP_DISABLE_LWS_LOG 0
#define WSFSP_CLIENT_PROTOCOL_COUNT 2
#define WSFSP_CLIENT_TIMEOUT (1 * 1000)

struct wsfsp_client
{
    volatile bool is_running;
    struct wsfsp_provider provider;
    struct wsfsp_client_protocol protocol;
    struct lws_context_creation_info info;
    struct lws_protocols protocols[WSFSP_CLIENT_PROTOCOL_COUNT];
    struct lws_context * context;
    struct lws * wsi;
};


struct wsfsp_client * wsfsp_client_create(
    struct wsfsp_provider * provider,
    void * user_data)
{
 	lws_set_log_level(WSFSP_DISABLE_LWS_LOG, NULL);
   
    struct wsfsp_client * client = malloc(sizeof(struct wsfsp_client));
    if (NULL != client)
    {
        client->is_running = true;
        wsfsp_client_protocol_init(&client->protocol, provider, user_data);

        memset(client->protocols, 0, sizeof(struct lws_protocols) * WSFSP_CLIENT_PROTOCOL_COUNT);
        client->protocols[0].name = "fs";
        wsfsp_client_protocol_init_lws(&client->protocol, &client->protocols[0]);

        memset(&client->info, 0, sizeof(struct lws_context_creation_info));
        client->info.port = CONTEXT_PORT_NO_LISTEN;
        client->info.protocols = client->protocols;
        client->info.uid = -1;
        client->info.gid = -1;

        client->context = lws_create_context(&client->info);
    }

    return client;
}

void wsfsp_client_dispose(
    struct wsfsp_client * client)
{
    lws_context_destroy(client->context);
    wsfsp_client_protocol_cleanup(&client->protocol);    
    free(client);
}

void wsfsp_client_connect(
    struct wsfsp_client * client,
    char const * url)
{
    struct wsfsp_url url_data;
    bool const success = wsfsp_url_init(&url_data, url);
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
        info.protocol = WSFSP_PROTOCOL;
        info.pwsi = &client->wsi;

        lws_client_connect_via_info(&info);

        wsfsp_url_cleanup(&url_data);
    }
}

void wsfsp_client_disconnect(
    struct wsfsp_client * client)
{
    (void) client;

    // ToDo: implement me
}

void wsfsp_client_settimeout(
    struct wsfsp_client * client,
    unsigned int timepoint)
{
    (void) client;
    (void) timepoint;

    // ToDo: implement me
}


void wsfsp_client_run(
    struct wsfsp_client * client)
{
    while (client->is_running)
    {
        lws_service(client->context, WSFSP_CLIENT_TIMEOUT);
    }
}

void wsfsp_client_shutdown(
    struct wsfsp_client * client)
{
    client->is_running = false;
}

