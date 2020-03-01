#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <webfuse/provider/client_protocol.h>
#include <webfuse/provider/client_config.h>
#include "webfuse/utils/ws_server.hpp"
#include "webfuse/mocks/mock_provider_client.hpp"

#include <cstring>
#include <thread>
#include <atomic>

using webfuse_test::WebsocketServer;
using webfuse_test::MockProviderClient;
using webfuse_test::IProviderClient;
using testing::_;
using testing::AtMost;
using testing::Invoke;

namespace
{

class ClientProtocolFixture
{
    ClientProtocolFixture(ClientProtocolFixture const &) = delete;
    ClientProtocolFixture& operator=(ClientProtocolFixture const &) = delete;
public:
    explicit ClientProtocolFixture(IProviderClient& client, bool enableAuthentication = false)
    {
        config = wfp_client_config_create();
        client.AttachTo(config, enableAuthentication);

        protocol = wfp_client_protocol_create(config);

        struct lws_protocols client_protocol;
        memset(&client_protocol, 0, sizeof(struct lws_protocols));
        wfp_client_protocol_init_lws(protocol, &client_protocol);

        server = new WebsocketServer(54321, &client_protocol, 1);
    }

    ~ClientProtocolFixture()
    {
        delete server;
        wfp_client_protocol_dispose(protocol);
        wfp_client_config_dispose(config);
    }

    void Connect()
    {
        wfp_client_protocol_connect(protocol, server->getContext(), "ws://localhost:54321/");
        server->waitForConnection();
    }

    void SendToClient(json_t * request)
    {
        server->sendMessage(request);
    }

    json_t * ReceiveMessageFromClient()
    {
        return server->receiveMessage();
    }

    void AwaitAuthentication(
        std::string const & expected_username,
        std::string const & expected_password)
    {
        json_t * request = server->receiveMessage();
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
        server->sendMessage(response);

        json_decref(request);
    }

    void AwaitAddFilesystem(std::string& filesystemName)
    {
        json_t * addFilesystemRequest = server->receiveMessage();
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

        server->sendMessage(response);

        json_decref(addFilesystemRequest);
    }

private:
    WebsocketServer * server;
    wfp_client_config * config;
    wfp_client_protocol * protocol;
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
}