#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <webfuse/provider/client_protocol.h>
#include <webfuse/provider/client_config.h>
#include "webfuse/utils/ws_server.hpp"

#include <cstring>
#include <thread>
#include <atomic>

using webfuse_test::WebsocketServer;
using testing::_;

namespace
{

class ClientProtocolTest: public ::testing::Test
{
public:
    ClientProtocolTest()
    : server(nullptr)
    , config(nullptr)
    , protocol(nullptr)
    {
        // empty
    }

protected:
    void SetUp()
    {
        config = wfp_client_config_create();
        protocol = wfp_client_protocol_create(config);

        struct lws_protocols client_protocol;
        memset(&client_protocol, 0, sizeof(struct lws_protocols));
        wfp_client_protocol_init_lws(protocol, &client_protocol);

        server = new WebsocketServer(54321, &client_protocol, 1);
    }

    void TearDown()
    {
        delete server;

        wfp_client_protocol_dispose(protocol);
        wfp_client_config_dispose(config);

    }

    void connect()
    {
        wfp_client_protocol_connect(protocol, server->getContext(), "ws://localhost:54321/");
        server->waitForConnection();
    }

    void awaitAddFilesystem(std::string& filesystemName)
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

    WebsocketServer * server;

private:
    wfp_client_config * config;
    wfp_client_protocol * protocol;
};



}


TEST_F(ClientProtocolTest, connect)
{
    connect();
    if (HasFatalFailure()) { return; }
}

TEST_F(ClientProtocolTest, getattr)
{
    connect();
    if (HasFatalFailure()) { return; }

    std::string filesystem;
    awaitAddFilesystem(filesystem);
    if (HasFatalFailure()) { return; }

    json_t * params = json_array();
    json_array_append_new(params, json_string(filesystem.c_str()));
    json_array_append_new(params, json_integer(1));
    json_t * request = json_object();
    json_object_set_new(request, "method", json_string("getattr"));
    json_object_set_new(request, "params", params);
    json_object_set_new(request, "id", json_integer(42));

    server->sendMessage(request);
    json_t * response = server->receiveMessage();
    ASSERT_TRUE(json_is_object(response));

    json_decref(response);
}