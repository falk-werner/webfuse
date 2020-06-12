#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "webfuse/adapter/client.h"
#include "webfuse/adapter/credentials.h"
#include "webfuse/core/protocol_names.h"
#include "webfuse/utils/ws_server.h"
#include "webfuse/mocks/mock_adapter_client_callback.hpp"

using webfuse_test::WsServer;
using webfuse_test::MockAdapterClientCallback;
using testing::_;

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

}

TEST(AdapterClient, GeneralUsage)
{
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

TEST(AdapterClient, CreateAndDispose)
{
    MockAdapterClientCallback callback;

    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_INIT, nullptr)).Times(1);
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CREATED, nullptr)).Times(1);
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_GET_TLS_CONFIG, _)).Times(1);
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CLEANUP, nullptr)).Times(1);

    wf_client * client = wf_client_create(
        callback.GetCallbackFn(), callback.GetUserData());

    wf_client_dispose(client);
}

TEST(AdapterClient, Connect)
{
    WsServer server(WF_PROTOCOL_NAME_PROVIDER_SERVER);
    MockAdapterClientCallback callback;

    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_INIT, nullptr)).Times(1);
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CREATED, nullptr)).Times(1);
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_GET_TLS_CONFIG, _)).Times(1);
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CLEANUP, nullptr)).Times(1);

    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CONNECTED, nullptr)).Times(1);
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_DISCONNECTED, nullptr)).Times(1);


    wf_client * client = wf_client_create(
        callback.GetCallbackFn(), callback.GetUserData());

    wf_client_connect(client, server.GetUrl().c_str());
    while (!server.IsConnected())
    {
        wf_client_service(client);
    }

    wf_client_disconnect(client);
    while (server.IsConnected())
    {
        wf_client_service(client);
    }

    wf_client_dispose(client);
}