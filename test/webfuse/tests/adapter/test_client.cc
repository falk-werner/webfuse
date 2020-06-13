#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "webfuse/adapter/client.h"
#include "webfuse/adapter/credentials.h"
#include "webfuse/adapter/credentials.h"
#include "webfuse/core/protocol_names.h"
#include "webfuse/utils/ws_server.h"
#include "webfuse/mocks/mock_adapter_client_callback.hpp"
#include "webfuse/utils/timeout_watcher.hpp"

using webfuse_test::WsServer;
using webfuse_test::MockAdapterClientCallback;
using webfuse_test::TimeoutWatcher;
using testing::_;
using testing::Invoke;
using testing::AnyNumber;

#define TIMEOUT (std::chrono::milliseconds(10 * 1000))

namespace
{

void GetCredentials(wf_client *, int, void * arg)
{
    auto * creds = reinterpret_cast<wf_credentials*>(arg);
    wf_credentials_set_type(creds, "username");
    wf_credentials_add(creds, "username", "Bob");
    wf_credentials_add(creds, "password", "secret");
}

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
    TimeoutWatcher watcher(TIMEOUT);

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
        watcher.check();
        wf_client_service(client);
    }

    wf_client_disconnect(client);
    while (server.IsConnected())
    {
        watcher.check();
        wf_client_service(client);
    }

    wf_client_dispose(client);
}

TEST(AdapterClient, Authenticate)
{
    TimeoutWatcher watcher(TIMEOUT);

    WsServer server(WF_PROTOCOL_NAME_PROVIDER_SERVER);
    MockAdapterClientCallback callback;

    EXPECT_CALL(callback, Invoke(_, _, _)).Times(AnyNumber());
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_AUTHENTICATE_GET_CREDENTIALS, _)).Times(1)
        .WillOnce(Invoke(GetCredentials));
    bool called = false;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_AUTHENTICATED, nullptr)).Times(1)
        .WillOnce(Invoke([&called] (wf_client *, int, void *) mutable {
            called = true;
        }));

    wf_client * client = wf_client_create(
        callback.GetCallbackFn(), callback.GetUserData());

    wf_client_connect(client, server.GetUrl().c_str());
    while (!server.IsConnected())
    {
        watcher.check();
        wf_client_service(client);
    }

    wf_client_authenticate(client);
    json_t * request = server.ReceiveMessage();
    while (nullptr == request)
    {
        watcher.check();
        wf_client_service(client);
        request = server.ReceiveMessage();
    }
    json_t * id = json_object_get(request, "id");
    ASSERT_TRUE(json_is_integer(id));
    json_t * method = json_object_get(request, "method");
    ASSERT_TRUE(json_is_string(method));
    ASSERT_STREQ("authenticate", json_string_value(method));
    json_t * params = json_object_get(request, "params");
    ASSERT_TRUE(json_is_array(params));
    ASSERT_EQ(2, json_array_size(params));
    json_t * type = json_array_get(params, 0);
    ASSERT_TRUE(json_is_string(type));
    json_t * creds = json_array_get(params, 1);
    ASSERT_TRUE(json_is_object(creds));
    json_t * username = json_object_get(creds, "username");
    ASSERT_TRUE(json_is_string(username));
    ASSERT_STREQ("Bob", json_string_value(username));
    json_t * password = json_object_get(creds, "password");
    ASSERT_TRUE(json_is_string(password));
    ASSERT_STREQ("secret", json_string_value(password));

    json_t * response = json_object();
    json_object_set(response, "id", id);
    json_object_set_new(response, "result", json_object());
    server.SendMessage(response);
    json_decref(request);
    while (!called) {
        watcher.check();
        wf_client_service(client);
    }

    wf_client_disconnect(client);
    while (server.IsConnected())
    {
        watcher.check();
        wf_client_service(client);
    }

    wf_client_dispose(client);
}