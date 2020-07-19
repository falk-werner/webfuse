#include "webfuse/test_util/server_protocol.hpp"
#include "webfuse/test_util/ws_client.hpp"
#include "webfuse/test_util/file.hpp"
#include "webfuse/test_util/json_doc.hpp"
#include "webfuse/impl/json/node.h"
#include "webfuse/mocks/mock_invokation_handler.hpp"
#include "webfuse/mocks/getattr_matcher.hpp"
#include "webfuse/protocol_names.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using webfuse_test::JsonDoc;
using webfuse_test::MockInvokationHander;
using webfuse_test::WsClient;
using webfuse_test::ServerProtocol;
using webfuse_test::File;
using webfuse_test::GetAttr;

using testing::_;
using testing::AnyNumber;
using testing::StrEq;
using testing::Return;

TEST(server_protocol, add_filesystem)
{
    ServerProtocol server;
    MockInvokationHander handler;
    EXPECT_CALL(handler, Invoke(StrEq("lookup"), _)).Times(AnyNumber());
    EXPECT_CALL(handler, Invoke(StrEq("getattr"), GetAttr(1))).Times(AnyNumber())
        .WillOnce(Return("{\"mode\": 420, \"type\": \"dir\"}"));
    WsClient client(handler, WF_PROTOCOL_NAME_PROVIDER_CLIENT);

    auto connected = client.Connect(server.GetPort(), WF_PROTOCOL_NAME_ADAPTER_SERVER, false);
    ASSERT_TRUE(connected);

    {
        std::string response_text = client.Invoke("{\"method\": \"authenticate\", \"params\": [\"username\", {\"username\": \"bob\", \"password\": \"secret\"}], \"id\": 23}");
        JsonDoc doc(response_text);
        wf_json const * response = doc.root();
        ASSERT_TRUE(wf_impl_json_is_object(response));
        wf_json const * result = wf_impl_json_object_get(response, "result");
        ASSERT_TRUE(wf_impl_json_is_object(result));
        wf_json const * id = wf_impl_json_object_get(response, "id");
        ASSERT_EQ(23, wf_impl_json_int_get(id));
    }

    {
        std::string response_text = client.Invoke("{\"method\": \"add_filesystem\", \"params\": [\"test\"], \"id\": 42}");
        JsonDoc doc(response_text);
        wf_json const * response = doc.root();
        ASSERT_TRUE(wf_impl_json_is_object(response));
        wf_json const * result = wf_impl_json_object_get(response, "result");
        ASSERT_TRUE(wf_impl_json_is_object(result));
        wf_json const * id = wf_impl_json_object_get(response, "id");
        ASSERT_EQ(42, wf_impl_json_int_get(id));
    }

    std::string base_dir = server.GetBaseDir();
    ASSERT_TRUE(File(base_dir).isDirectory());
    File file(base_dir + "/test");
    ASSERT_TRUE(file.isDirectory());

    auto disconnected = client.Disconnect();
    ASSERT_TRUE(disconnected);
}

TEST(server_protocol, add_filesystem_fail_without_authentication)
{
    ServerProtocol server;
    MockInvokationHander handler;
    EXPECT_CALL(handler, Invoke(StrEq("lookup"), _)).Times(AnyNumber());
    EXPECT_CALL(handler, Invoke(StrEq("getattr"), GetAttr(1))).Times(AnyNumber())
        .WillOnce(Return("{\"mode\": 420, \"type\": \"dir\"}"));
    WsClient client(handler, WF_PROTOCOL_NAME_PROVIDER_CLIENT);

    auto connected = client.Connect(server.GetPort(), WF_PROTOCOL_NAME_ADAPTER_SERVER, false);
    ASSERT_TRUE(connected);

    {
        std::string response_text = client.Invoke("{\"method\": \"add_filesystem\", \"params\": [\"test\"], \"id\": 42}");
        JsonDoc doc(response_text);
        wf_json const * response = doc.root();
        ASSERT_TRUE(wf_impl_json_is_object(response));
        wf_json const * error = wf_impl_json_object_get(response, "error");
        ASSERT_TRUE(wf_impl_json_is_object(error));
        wf_json const * id = wf_impl_json_object_get(response, "id");
        ASSERT_EQ(42, wf_impl_json_int_get(id));
    }

    auto disconnected = client.Disconnect();
    ASSERT_TRUE(disconnected);
}

TEST(server_protocol, authenticate_fail_wrong_credentials)
{
    ServerProtocol server;
    MockInvokationHander handler;
    EXPECT_CALL(handler, Invoke(StrEq("lookup"), _)).Times(AnyNumber());
    EXPECT_CALL(handler, Invoke(StrEq("getattr"), GetAttr(1))).Times(AnyNumber())
        .WillOnce(Return("{\"mode\": 420, \"type\": \"dir\"}"));
    WsClient client(handler, WF_PROTOCOL_NAME_PROVIDER_CLIENT);

    auto connected = client.Connect(server.GetPort(), WF_PROTOCOL_NAME_ADAPTER_SERVER, false);
    ASSERT_TRUE(connected);

    {
        std::string response_text = client.Invoke("{\"method\": \"authenticate\", \"params\": [\"username\", {\"username\": \"alice\", \"password\": \"cheshire\"}], \"id\": 23}");
        JsonDoc doc(response_text);
        wf_json const * response = doc.root();
        ASSERT_TRUE(wf_impl_json_is_object(response));
        wf_json const * error = wf_impl_json_object_get(response, "error");
        ASSERT_TRUE(wf_impl_json_is_object(error));
        wf_json const * id = wf_impl_json_object_get(response, "id");
        ASSERT_EQ(23, wf_impl_json_int_get(id));
    }

    auto disconnected = client.Disconnect();
    ASSERT_TRUE(disconnected);
}
