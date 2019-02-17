#include "wsfsp/client.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "wsfsp/provider.h"

struct wsfsp_client
{
    volatile bool is_running;
    struct wsfsp_provider provider;
    void * user_data;
};


struct wsfsp_client * wsfsp_client_create(
    struct wsfsp_provider * provider,
    void * user_data)
{
    struct wsfsp_client * client = malloc(sizeof(struct wsfsp_client));
    if (NULL != client)
    {
        client->is_running = true;
        client->user_data = user_data;
        memcpy(&client->provider, provider, sizeof(struct wsfsp_provider));
    }

    return client;
}

void wsfsp_client_dispose(
    struct wsfsp_client * client)
{
    free(client);
}

void wsfsp_client_connect(
    struct wsfsp_client * client,
    char const * url)
{
    (void) client;
    (void) url;

    // ToDo: implement me
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
        // ToDo: implement me
        break;
    }
}

void wsfsp_client_shutdown(
    struct wsfsp_client * client)
{
    client->is_running = false;
}

