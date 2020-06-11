#include <gtest/gtest.h>

#include "webfuse/adapter/client.h"
#include "webfuse/adapter/credentials.h"
#include "webfuse/utils/threaded_ws_server.h"

using webfuse_test::ThreadedWsServer;

namespace
{

enum class connection_state
{
    disconnected,
    connected,
    connecting
};

struct context
{
    connection_state state;
};

void callback(
    wf_client * client,
    int reason,
    void * args)
{
    auto * ctx = reinterpret_cast<context*>(wf_client_get_userdata(client));

    switch (reason)
    {
        case WF_CLIENT_CREATED:
            ctx->state = connection_state::connecting;
            wf_client_connect(client, "ws://dummy-server/");
            break;
        case WF_CLIENT_CONNECTED:
            ctx->state = connection_state::connected;
            wf_client_authenticate(client);
            break;
        case WF_CLIENT_AUTHENTICATED:
            wf_client_add_filesystem(client, ".", "test");
            break;
        case WF_CLIENT_AUTHENTICATION_FAILED:
            wf_client_disconnect(client);
            break;
        case WF_CLIENT_AUTHENTICATE_GET_CREDENTIALS:
            {
                auto * credentials = reinterpret_cast<wf_credentials*>(args);
                wf_credentials_set_type(credentials, "username");
                wf_credentials_add(credentials, "user", "bob");
                wf_credentials_add(credentials, "password", "secret");
            }
            break;
        case WF_CLIENT_FILESYSTEM_ADDED:
            // operational
            break;
        case WF_CLIENT_FILESYSTEM_ADD_FAILED:
            wf_client_disconnect(client);
            break;
        case WF_CLIENT_DISCONNECTED:
            ctx->state = connection_state::disconnected;
            break;
        default:
            break;
    }
}

void callback2(
    wf_client * client,
    int reason,
    void * args)
{
    auto * ctx = reinterpret_cast<context*>(wf_client_get_userdata(client));

    switch (reason)
    {
        case WF_CLIENT_CREATED:
            ctx->state = connection_state::connecting;
            break;
        case WF_CLIENT_CONNECTED:
            ctx->state = connection_state::connected;
            break;
        case WF_CLIENT_DISCONNECTED:
            ctx->state = connection_state::disconnected;
            break;
        default:
            break;
    }
}


}

TEST(client, general_usage)
{
    ThreadedWsServer server(54321);

    context ctx;
    ctx.state = connection_state::connecting;

    wf_client * client = wf_client_create(
        &callback, reinterpret_cast<void*>(&ctx));

    while (ctx.state != connection_state::disconnected)
    {
        wf_client_service(client);
    }

    wf_client_dispose(client);
}

TEST(client, connect)
{
    ThreadedWsServer server(54321);

    context ctx;
    ctx.state = connection_state::connecting;

    wf_client * client = wf_client_create(
        &callback2, reinterpret_cast<void*>(&ctx));

    wf_client_connect(client, "ws://localhost:54321/");
    while (ctx.state != connection_state::connected)
    {
        wf_client_service(client);
    }

    wf_client_disconnect(client);
    while (ctx.state != connection_state::disconnected)
    {
        wf_client_service(client);
    }

    wf_client_dispose(client);
}