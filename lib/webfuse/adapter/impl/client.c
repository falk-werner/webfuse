#include "webfuse/adapter/impl/client.h"

#include <stdlib.h>

struct wf_client
{
    wf_client_callback_fn * callback;
    void * user_data;
};

struct wf_client *
wf_impl_client_create(
    wf_client_callback_fn * callback,
    void * user_data)
{
    struct wf_client * client = malloc(sizeof(struct wf_client));
    client->callback = callback;
    client->user_data = user_data;

    client->callback(client, WF_CLIENT_CREATED, NULL);

    return client;
}

void
wf_impl_client_dispose(
    struct wf_client * client)
{
    client->callback(client, WF_CLIENT_DISPOSING, NULL);
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
    (void) client;
}

void
wf_impl_client_interrupt(
    struct wf_client * client)
{
    (void) client;
}

void
wf_impl_client_connect(
    struct wf_client * client,
    char const * url)
{
    (void) url;
    client->callback(client, WF_CLIENT_DISCONNECTED, NULL);
}

void
wf_impl_client_disconnect(
    struct wf_client * client)
{
    (void) client;
}

void
wf_impl_client_authenticate(
    struct wf_client * client)
{
    client->callback(client, WF_CLIENT_AUTHENTICATION_FAILED, NULL);
}

void
wf_impl_client_add_filesystem(
    struct wf_client * client,
    char const * local_path,
    char const * name)
{
    (void) local_path;
    (void) name;

    client->callback(client, WF_CLIENT_FILESYSTEM_ADD_FAILED, NULL);
}
