#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <webfuse/provider/client_protocol.h>
#include <webfuse/provider/client_config.h>
#include "webfuse/utils/ws_server.h"
#include "webfuse/mocks/mock_provider_client.hpp"
#include "webfuse/core/protocol_names.h"
#include "webfuse/utils/timeout_watcher.hpp"

#include <libwebsockets.h>

#include <cstring>
#include <thread>
#include <atomic>

using webfuse_test::WsServer;
using webfuse_test::MockProviderClient;
using webfuse_test::IProviderClient;
using webfuse_test::TimeoutWatcher;
using testing::_;
using testing::AtMost;
using testing::Invoke;

#define DEFAULT_TIMEOUT (std::chrono::milliseconds(5 * 1000))

namespace
{

class ClientProtocolFixture
{
    ClientProtocolFixture(ClientProtocolFixture const &) = delete;
    ClientProtocolFixture& operator=(ClientProtocolFixture const &) = delete;
public:
    explicit ClientProtocolFixture(IProviderClient& client, bool enableAuthentication = false)
    {
        server = new WsServer(WF_PROTOCOL_NAME_ADAPTER_SERVER);

        config = wfp_client_config_create();
        client.AttachTo(config, enableAuthentication);
        protocol = wfp_client_protocol_create(config);

        memset(protocols, 0, sizeof(struct lws_protocols) * 2);
        wfp_client_protocol_init_lws(protocol, protocols);

        memset(&info, 0, sizeof(struct lws_context_creation_info));
        info.port = CONTEXT_PORT_NO_LISTEN;
        info.protocols = protocols;
        info.uid = -1;
        info.gid = -1;

        context = lws_create_context(&info);
    }

    ~ClientProtocolFixture()
    {
        lws_context_destroy(context);
        wfp_client_protocol_dispose(protocol);
        wfp_client_config_dispose(config);
        delete server;
    }

    void Connect()
    {
        TimeoutWatcher watcher(DEFAULT_TIMEOUT);

        wfp_client_protocol_connect(protocol, context, server->GetUrl().c_str());        
        while (!server->IsConnected())
        {
            watcher.check();
            lws_service(context, 0);
        }
    }

    void Disconnect()
    {
        wfp_client_protocol_disconnect(protocol);
    }

    void SendToClient(json_t * request)
    {
        server->SendMessage(request);
    }

    json_t * ReceiveMessageFromClient()
    {
        TimeoutWatcher watcher(DEFAULT_TIMEOUT);
        json_t * result = server->ReceiveMessage();
        while (nullptr == result)
        {
            watcher.check();
            lws_service(context, 0);
            result = server->ReceiveMessage();
        }

        return result;
    }

    void AwaitAuthentication(
        std::string const & expected_username,
        std::string const & expected_password)
    {
        json_t * request = ReceiveMessageFromClient();
        ASSERT_TRUE(json_is_object(request));

        json_t * method = json_object_get(request, "method");
        ASSERT_TRUE(json_is_string(method));
        ASSERT_STREQ("authenticate", json_string_value(method));

        json_t * id = json_object_get(request, "id");
        ASSERT_TRUE(json_is_integer(id));

        json_t * params = json_object_get(request, "params");
        ASSERT_TRUE(json_is_array(params));
        ASSERT_EQ(2, json_array_size(params));

        json_t * type = json_array_get(params, 0);
        ASSERT_TRUE(json_is_string(type));
        ASSERT_STREQ("username", json_string_value(type));

        json_t * credentials = json_array_get(params, 1);
        ASSERT_TRUE(json_is_object(credentials));

        json_t * username = json_object_get(credentials, "username");
        ASSERT_TRUE(json_is_string(username));
        ASSERT_STREQ(expected_username.c_str(), json_string_value(username));
        
        json_t * password = json_object_get(credentials, "password");
        ASSERT_TRUE(json_is_string(password));
        ASSERT_STREQ(expected_password.c_str(), json_string_value(password));

        json_t * response = json_object();
        json_object_set_new(response, "result", json_object());
        json_object_set(response, "id", id);
        SendToClient(response);

        json_decref(request);
    }

    void AwaitAddFilesystem(std::string& filesystemName)
    {
        json_t * addFilesystemRequest = ReceiveMessageFromClient();
        ASSERT_NE(nullptr, addFilesystemRequest);
        ASSERT_TRUE(json_is_object(addFilesystemRequest));

        json_t * method = json_object_get(addFilesystemRequest, "method");
        ASSERT_TRUE(json_is_string(method));
        ASSERT_STREQ("add_filesystem", json_string_value(method));

        json_t * params = json_object_get(addFilesystemRequest, "params");
        ASSERT_TRUE(json_is_array(params));
        ASSERT_EQ(1, json_array_size(params));

        json_t * filesystem = json_array_get(params, 0);
        ASSERT_TRUE(json_is_string(filesystem));
        filesystemName = json_string_value(filesystem);

        json_t * id = json_object_get(addFilesystemRequest, "id");
        ASSERT_TRUE(json_is_integer(id));

        json_t * response = json_object();
        json_t * result = json_object();
        json_object_set(result, "id", filesystem);
        json_object_set_new(response, "result", result);
        json_object_set(response, "id", id);

        SendToClient(response);

        json_decref(addFilesystemRequest);
    }

private:
    WsServer * server;
    wfp_client_config * config;
    wfp_client_protocol * protocol;
    struct lws_context_creation_info info;
    struct lws_protocols protocols[2];
    struct lws_context * context;

};

void GetCredentials(wfp_credentials * credentials)
{
    wfp_credentials_set_type(credentials, "username");
    wfp_credentials_add(credentials, "username", "bob");
    wfp_credentials_add(credentials, "password", "secret");
}

}

TEST(client_protocol, connect)
{
    MockProviderClient provider;
    ClientProtocolFixture fixture(provider);

    EXPECT_CALL(provider, OnConnected()).Times(AtMost(1));
    EXPECT_CALL(provider, OnDisconnected()).Times(1);

    fixture.Connect();
    if (HasFatalFailure()) { return; }

    std::string filesystem;
    fixture.AwaitAddFilesystem(filesystem);
    if (HasFatalFailure()) { return; }
}

TEST(client_protocol, disconnect_without_connect)
{
    MockProviderClient provider;
    ClientProtocolFixture fixture(provider);

    EXPECT_CALL(provider, OnDisconnected()).Times(1);

    fixture.Disconnect();
}

TEST(client_protocol, connect_with_username_authentication)
{
    MockProviderClient provider;
    ClientProtocolFixture fixture(provider, true);

    EXPECT_CALL(provider, OnConnected()).Times(AtMost(1));
    EXPECT_CALL(provider, OnDisconnected()).Times(1);
    EXPECT_CALL(provider, GetCredentials(_)).Times(1).WillOnce(Invoke(GetCredentials));
    
    fixture.Connect();
    if (HasFatalFailure()) { return; }

    fixture.AwaitAuthentication("bob", "secret");
    if (HasFatalFailure()) { return; }

    std::string filesystem;
    fixture.AwaitAddFilesystem(filesystem);
    if (HasFatalFailure()) { return; }
}

TEST(client_protocol, getattr)
{
    MockProviderClient provider;
    ClientProtocolFixture fixture(provider);

    EXPECT_CALL(provider, OnConnected()).Times(1);
    EXPECT_CALL(provider, OnDisconnected()).Times(1);
    EXPECT_CALL(provider, GetAttr(1, _)).Times(1);

    fixture.Connect();
    if (HasFatalFailure()) { return; }

    std::string filesystem;
    fixture.AwaitAddFilesystem(filesystem);
    if (HasFatalFailure()) { return; }

    json_t * params = json_array();
    json_array_append_new(params, json_string(filesystem.c_str()));
    json_array_append_new(params, json_integer(1));
    json_t * request = json_object();
    json_object_set_new(request, "method", json_string("getattr"));
    json_object_set_new(request, "params", params);
    json_object_set_new(request, "id", json_integer(42));

    fixture.SendToClient(request);
    json_t * response = fixture.ReceiveMessageFromClient();
    ASSERT_TRUE(json_is_object(response));

    json_decref(response);

    fixture.Disconnect();
}