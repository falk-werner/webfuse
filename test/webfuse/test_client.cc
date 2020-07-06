#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "webfuse/test_util/adapter_client.hpp"
#include "webfuse/client_tlsconfig.h"
#include "webfuse/credentials.h"
#include "webfuse/protocol_names.h"
#include "webfuse/test_util/ws_server.hpp"
#include "webfuse/mocks/mock_adapter_client_callback.hpp"
#include "webfuse/mocks/mock_invokation_handler.hpp"
#include "webfuse/test_util/file.hpp"
#include "webfuse/mocks/lookup_matcher.hpp"
#include "webfuse/mocks/open_matcher.hpp"
#include "webfuse/mocks/getattr_matcher.hpp"

#include <future>
#include <chrono>

using webfuse_test::AdapterClient;
using webfuse_test::WsServer;
using webfuse_test::MockInvokationHander;
using webfuse_test::MockAdapterClientCallback;
using webfuse_test::File;
using webfuse_test::GetAttr;
using webfuse_test::Open;
using webfuse_test::Lookup;
using testing::_;
using testing::Invoke;
using testing::AnyNumber;
using testing::AtMost;
using testing::Return;
using testing::Throw;
using testing::StrEq;

#define TIMEOUT (std::chrono::seconds(30))

namespace
{

void GetCredentials(wf_client *, int, void * arg)
{
    auto * creds = reinterpret_cast<wf_credentials*>(arg);
    wf_credentials_set_type(creds, "username");
    wf_credentials_add(creds, "username", "Bob");
    wf_credentials_add(creds, "password", "secret");
}

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
    MockInvokationHander handler;
    WsServer server(handler, WF_PROTOCOL_NAME_PROVIDER_SERVER);
    EXPECT_CALL(handler, Invoke(_,_)).Times(0);

    MockAdapterClientCallback callback;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_INIT, nullptr)).Times(1);
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CREATED, nullptr)).Times(1);
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_GET_TLS_CONFIG, _)).Times(1);
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CLEANUP, nullptr)).Times(1);

    std::promise<void> connected;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { connected.set_value(); }));

    std::promise<void> disconnected;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_DISCONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { disconnected.set_value(); }));

    AdapterClient client(callback.GetCallbackFn(), callback.GetUserData(), server.GetUrl());

    client.Connect();
    ASSERT_EQ(std::future_status::ready, connected.get_future().wait_for(TIMEOUT));

    client.Disconnect();
    ASSERT_EQ(std::future_status::ready, disconnected.get_future().wait_for(TIMEOUT));
}

TEST(AdapterClient, IgnoreNonJsonMessage)
{
    MockInvokationHander handler;
    WsServer server(handler, WF_PROTOCOL_NAME_PROVIDER_SERVER);
    EXPECT_CALL(handler, Invoke(_,_)).Times(0);

    MockAdapterClientCallback callback;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_INIT, nullptr)).Times(1);
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CREATED, nullptr)).Times(1);
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_GET_TLS_CONFIG, _)).Times(1);
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CLEANUP, nullptr)).Times(1);

    std::promise<void> connected;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { connected.set_value(); }));

    std::promise<void> disconnected;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_DISCONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { disconnected.set_value(); }));

    AdapterClient client(callback.GetCallbackFn(), callback.GetUserData(), server.GetUrl());

    client.Connect();
    ASSERT_EQ(std::future_status::ready, connected.get_future().wait_for(TIMEOUT));

    server.SendMessage("brummni");

    client.Disconnect();
    ASSERT_EQ(std::future_status::ready, disconnected.get_future().wait_for(TIMEOUT));
}


TEST(AdapterClient, IgnoreInvalidJsonMessage)
{
    MockInvokationHander handler;
    WsServer server(handler, WF_PROTOCOL_NAME_PROVIDER_SERVER);
    EXPECT_CALL(handler, Invoke(_,_)).Times(0);

    MockAdapterClientCallback callback;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_INIT, nullptr)).Times(1);
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CREATED, nullptr)).Times(1);
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_GET_TLS_CONFIG, _)).Times(1);
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CLEANUP, nullptr)).Times(1);

    std::promise<void> connected;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { connected.set_value(); }));

    std::promise<void> disconnected;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_DISCONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { disconnected.set_value(); }));

    AdapterClient client(callback.GetCallbackFn(), callback.GetUserData(), server.GetUrl());

    client.Connect();
    ASSERT_EQ(std::future_status::ready, connected.get_future().wait_for(TIMEOUT));

    json_t * invalid_request = json_object();
    server.SendMessage(invalid_request);

    client.Disconnect();
    ASSERT_EQ(std::future_status::ready, disconnected.get_future().wait_for(TIMEOUT));
}

TEST(AdapterClient, ConnectWithTls)
{
    MockInvokationHander handler;
    WsServer server(handler, WF_PROTOCOL_NAME_PROVIDER_SERVER, 0, true);
    EXPECT_CALL(handler, Invoke(_,_)).Times(0);

    MockAdapterClientCallback callback;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_INIT, nullptr)).Times(1);
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CREATED, nullptr)).Times(1);
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_GET_TLS_CONFIG, _)).Times(1)
        .WillOnce(Invoke([](wf_client *, int, void * arg) {
            auto * tls = reinterpret_cast<wf_client_tlsconfig*>(arg);
            wf_client_tlsconfig_set_keypath (tls, "client-key.pem");
            wf_client_tlsconfig_set_certpath(tls, "client-cert.pem");
            wf_client_tlsconfig_set_cafilepath(tls, "server-cert.pem");
        }));
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CLEANUP, nullptr)).Times(1);

    std::promise<void> connected;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { connected.set_value(); }));

    std::promise<void> disconnected;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_DISCONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { disconnected.set_value(); }));

    AdapterClient client(callback.GetCallbackFn(), callback.GetUserData(), server.GetUrl());

    client.Connect();
    ASSERT_EQ(std::future_status::ready, connected.get_future().wait_for(TIMEOUT));

    client.Disconnect();
    ASSERT_EQ(std::future_status::ready, disconnected.get_future().wait_for(TIMEOUT));
}

TEST(AdapterClient, FailedToConnectInvalidPort)
{
    MockAdapterClientCallback callback;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_INIT, nullptr)).Times(1);
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CREATED, nullptr)).Times(1);
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_GET_TLS_CONFIG, _)).Times(1);
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CLEANUP, nullptr)).Times(1);

    std::promise<void> disconnected;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_DISCONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { disconnected.set_value(); }));

    AdapterClient client(callback.GetCallbackFn(), callback.GetUserData(), "ws://localhost:4/");

    client.Connect();
    ASSERT_EQ(std::future_status::ready, disconnected.get_future().wait_for(TIMEOUT));
}


TEST(AdapterClient, Authenticate)
{
    MockInvokationHander handler;
    WsServer server(handler, WF_PROTOCOL_NAME_PROVIDER_SERVER);
    EXPECT_CALL(handler, Invoke(StrEq("authenticate"),_)).Times(1)
        .WillOnce(Return("{}"));

    MockAdapterClientCallback callback;
    EXPECT_CALL(callback, Invoke(_, _, _)).Times(AnyNumber());

    std::promise<void> connected;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { connected.set_value(); }));

    std::promise<void> disconnected;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_DISCONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { disconnected.set_value(); }));

    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_AUTHENTICATE_GET_CREDENTIALS, _)).Times(1)
        .WillOnce(Invoke(GetCredentials));

    std::promise<void> authenticated;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_AUTHENTICATED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { authenticated.set_value(); }));

    AdapterClient client(callback.GetCallbackFn(), callback.GetUserData(), server.GetUrl());

    client.Connect();
    ASSERT_EQ(std::future_status::ready, connected.get_future().wait_for(TIMEOUT));

    client.Authenticate();
    ASSERT_EQ(std::future_status::ready, authenticated.get_future().wait_for(TIMEOUT));

    client.Disconnect();
    ASSERT_EQ(std::future_status::ready, disconnected.get_future().wait_for(TIMEOUT));
}

TEST(AdapterClient, AuthenticateFailedWithoutConnect)
{
    MockAdapterClientCallback callback;
    EXPECT_CALL(callback, Invoke(_, _, _)).Times(AnyNumber());
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_AUTHENTICATE_GET_CREDENTIALS, _)).Times(1)
        .WillOnce(Invoke(GetCredentials));
    std::promise<void> called;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_AUTHENTICATION_FAILED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable {
            called.set_value();
        }));

    AdapterClient client(callback.GetCallbackFn(), callback.GetUserData(), "");

    client.Authenticate();
    ASSERT_EQ(std::future_status::ready, called.get_future().wait_for(TIMEOUT));
}

TEST(AdapterClient, AuthenticationFailed)
{
    MockInvokationHander handler;
    WsServer server(handler, WF_PROTOCOL_NAME_PROVIDER_SERVER);
    EXPECT_CALL(handler, Invoke(StrEq("authenticate"),_)).Times(1)
        .WillOnce(Throw(std::runtime_error("authentication failed")));

    MockAdapterClientCallback callback;
    EXPECT_CALL(callback, Invoke(_, _, _)).Times(AnyNumber());

    std::promise<void> connected;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { connected.set_value(); }));

    std::promise<void> disconnected;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_DISCONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { disconnected.set_value(); }));

    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_AUTHENTICATE_GET_CREDENTIALS, _)).Times(1)
        .WillOnce(Invoke(GetCredentials));

    std::promise<void> called;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_AUTHENTICATION_FAILED, nullptr)).Times(1)
        .WillOnce(Invoke([&called] (wf_client *, int, void *) mutable {
            called.set_value();
        }));

    AdapterClient client(callback.GetCallbackFn(), callback.GetUserData(), server.GetUrl());

    client.Connect();
    ASSERT_EQ(std::future_status::ready, connected.get_future().wait_for(TIMEOUT));

    client.Authenticate();
    ASSERT_EQ(std::future_status::ready, called.get_future().wait_for(TIMEOUT));

    client.Disconnect();
    ASSERT_EQ(std::future_status::ready, disconnected.get_future().wait_for(TIMEOUT));
}

TEST(AdapterClient, AddFileSystem)
{
    MockInvokationHander handler;
    WsServer server(handler, WF_PROTOCOL_NAME_PROVIDER_SERVER);
    EXPECT_CALL(handler, Invoke(StrEq("add_filesystem"),_)).Times(1)
        .WillOnce(Return("{\"id\": \"test\"}"));
    EXPECT_CALL(handler, Invoke(StrEq("lookup"), _)).Times(AnyNumber())
        .WillRepeatedly(Throw(std::runtime_error("unknown")));

    MockAdapterClientCallback callback;
    EXPECT_CALL(callback, Invoke(_, _, _)).Times(AnyNumber());

    std::promise<void> connected;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { connected.set_value(); }));

    std::promise<void> disconnected;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_DISCONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { disconnected.set_value(); }));

    std::promise<void> called;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_FILESYSTEM_ADDED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable {
            called.set_value();
        }));

    AdapterClient client(callback.GetCallbackFn(), callback.GetUserData(), server.GetUrl());

    client.Connect();
    ASSERT_EQ(std::future_status::ready, connected.get_future().wait_for(TIMEOUT));

    client.AddFileSystem();
    ASSERT_EQ(std::future_status::ready, called.get_future().wait_for(TIMEOUT));

    client.Disconnect();
    ASSERT_EQ(std::future_status::ready, disconnected.get_future().wait_for(TIMEOUT));
}

TEST(AdapterClient, FailToAddFileSystemTwice)
{
    MockInvokationHander handler;
    WsServer server(handler, WF_PROTOCOL_NAME_PROVIDER_SERVER);
    EXPECT_CALL(handler, Invoke(StrEq("add_filesystem"),_)).Times(1)
        .WillOnce(Return("{\"id\": \"test\"}"));
    EXPECT_CALL(handler, Invoke(StrEq("lookup"), _)).Times(AnyNumber())
        .WillRepeatedly(Throw(std::runtime_error("unknown")));

    MockAdapterClientCallback callback;
    EXPECT_CALL(callback, Invoke(_, _, _)).Times(AnyNumber());

    std::promise<void> connected;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { connected.set_value(); }));

    std::promise<void> disconnected;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_DISCONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { disconnected.set_value(); }));

    std::promise<void> filesystem_added;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_FILESYSTEM_ADDED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable {
            filesystem_added.set_value();
        }));

    std::promise<void> filesystem_add_failed;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_FILESYSTEM_ADD_FAILED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable {
            filesystem_add_failed.set_value();
        }));

    AdapterClient client(callback.GetCallbackFn(), callback.GetUserData(), server.GetUrl());

    client.Connect();
    ASSERT_EQ(std::future_status::ready, connected.get_future().wait_for(TIMEOUT));

    client.AddFileSystem();
    ASSERT_EQ(std::future_status::ready, filesystem_added.get_future().wait_for(TIMEOUT));

    client.AddFileSystem();
    ASSERT_EQ(std::future_status::ready, filesystem_add_failed.get_future().wait_for(TIMEOUT));

    client.Disconnect();
    ASSERT_EQ(std::future_status::ready, disconnected.get_future().wait_for(TIMEOUT));
}

TEST(AdapterClient, FailToAddFileSystemMissingId)
{
    MockInvokationHander handler;
    WsServer server(handler, WF_PROTOCOL_NAME_PROVIDER_SERVER);
    EXPECT_CALL(handler, Invoke(StrEq("add_filesystem"),_)).Times(1)
        .WillOnce(Return("{}"));
    EXPECT_CALL(handler, Invoke(StrEq("lookup"), _)).Times(AnyNumber())
        .WillRepeatedly(Throw(std::runtime_error("unknown")));

    MockAdapterClientCallback callback;
    EXPECT_CALL(callback, Invoke(_, _, _)).Times(AnyNumber());

    std::promise<void> connected;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { connected.set_value(); }));

    std::promise<void> disconnected;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_DISCONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { disconnected.set_value(); }));

    std::promise<void> filesystem_add_failed;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_FILESYSTEM_ADD_FAILED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable {
            filesystem_add_failed.set_value();
        }));

    AdapterClient client(callback.GetCallbackFn(), callback.GetUserData(), server.GetUrl());

    client.Connect();
    ASSERT_EQ(std::future_status::ready, connected.get_future().wait_for(TIMEOUT));

    client.AddFileSystem();
    ASSERT_EQ(std::future_status::ready, filesystem_add_failed.get_future().wait_for(TIMEOUT));

    client.Disconnect();
    ASSERT_EQ(std::future_status::ready, disconnected.get_future().wait_for(TIMEOUT));
}

TEST(AdapterClient, FailToAddFileSystemIdNotString)
{
    MockInvokationHander handler;
    WsServer server(handler, WF_PROTOCOL_NAME_PROVIDER_SERVER);
    EXPECT_CALL(handler, Invoke(StrEq("add_filesystem"),_)).Times(1)
        .WillOnce(Return("{\"id\": 42}"));
    EXPECT_CALL(handler, Invoke(StrEq("lookup"), _)).Times(AnyNumber())
        .WillRepeatedly(Throw(std::runtime_error("unknown")));

    MockAdapterClientCallback callback;
    EXPECT_CALL(callback, Invoke(_, _, _)).Times(AnyNumber());

    std::promise<void> connected;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { connected.set_value(); }));

    std::promise<void> disconnected;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_DISCONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { disconnected.set_value(); }));

    std::promise<void> filesystem_add_failed;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_FILESYSTEM_ADD_FAILED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable {
            filesystem_add_failed.set_value();
        }));

    AdapterClient client(callback.GetCallbackFn(), callback.GetUserData(), server.GetUrl());

    client.Connect();
    ASSERT_EQ(std::future_status::ready, connected.get_future().wait_for(TIMEOUT));

    client.AddFileSystem();
    ASSERT_EQ(std::future_status::ready, filesystem_add_failed.get_future().wait_for(TIMEOUT));

    client.Disconnect();
    ASSERT_EQ(std::future_status::ready, disconnected.get_future().wait_for(TIMEOUT));
}


TEST(AdapterClient, AddFileSystemFailed)
{
    MockInvokationHander handler;
    WsServer server(handler, WF_PROTOCOL_NAME_PROVIDER_SERVER);
    EXPECT_CALL(handler, Invoke(StrEq("add_filesystem"),_)).Times(1)
        .WillOnce(Throw(std::runtime_error("failed")));

    MockAdapterClientCallback callback;
    EXPECT_CALL(callback, Invoke(_, _, _)).Times(AnyNumber());

    std::promise<void> connected;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { connected.set_value(); }));

    std::promise<void> disconnected;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_DISCONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { disconnected.set_value(); }));

    std::promise<void> called;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_FILESYSTEM_ADD_FAILED, nullptr)).Times(1)
        .WillOnce(Invoke([&called] (wf_client *, int, void *) mutable {
            called.set_value();
        }));

    AdapterClient client(callback.GetCallbackFn(), callback.GetUserData(), server.GetUrl());

    client.Connect();
    ASSERT_EQ(std::future_status::ready, connected.get_future().wait_for(TIMEOUT));

    client.AddFileSystem();
    ASSERT_EQ(std::future_status::ready, called.get_future().wait_for(TIMEOUT));

    client.Disconnect();
    ASSERT_EQ(std::future_status::ready, disconnected.get_future().wait_for(TIMEOUT));
}

TEST(AdapterClient, LookupFile)
{
    MockInvokationHander handler;
    WsServer server(handler, WF_PROTOCOL_NAME_PROVIDER_SERVER);
    EXPECT_CALL(handler, Invoke(StrEq("add_filesystem"),_)).Times(1)
        .WillOnce(Return("{\"id\": \"test\"}"));
    EXPECT_CALL(handler, Invoke(StrEq("lookup"), _)).Times(AnyNumber())
        .WillRepeatedly(Throw(std::runtime_error("unknown")));
    EXPECT_CALL(handler, Invoke(StrEq("lookup"), Lookup(1, "Hello.txt"))).Times(AnyNumber())
        .WillRepeatedly(Return("{\"inode\": 2, \"mode\": 420, \"type\": \"file\", \"size\": 42}"));
    EXPECT_CALL(handler, Invoke(StrEq("getattr"), GetAttr(1))).Times(AnyNumber())
        .WillOnce(Return("{\"mode\": 420, \"type\": \"dir\"}"));

    MockAdapterClientCallback callback;
    EXPECT_CALL(callback, Invoke(_, _, _)).Times(AnyNumber());

    std::promise<void> connected;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { connected.set_value(); }));

    std::promise<void> disconnected;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_DISCONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { disconnected.set_value(); }));

    std::promise<void> called;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_FILESYSTEM_ADDED, nullptr)).Times(1)
        .WillOnce(Invoke([&called] (wf_client *, int, void *) mutable {
            called.set_value();
        }));

    AdapterClient client(callback.GetCallbackFn(), callback.GetUserData(), server.GetUrl());

    client.Connect();
    ASSERT_EQ(std::future_status::ready, connected.get_future().wait_for(TIMEOUT));

    client.AddFileSystem();
    ASSERT_EQ(std::future_status::ready, called.get_future().wait_for(TIMEOUT));

    std::string base_dir = client.GetDir();
    ASSERT_TRUE(File(base_dir).isDirectory());
    std::string file_name = client.GetDir() + "/Hello.txt";
    File file(file_name);
    ASSERT_TRUE(file.isFile());

    client.Disconnect();
    ASSERT_EQ(std::future_status::ready, disconnected.get_future().wait_for(TIMEOUT));
}

TEST(AdapterClient, ReadFile)
{
    MockInvokationHander handler;
    WsServer server(handler, WF_PROTOCOL_NAME_PROVIDER_SERVER);
    EXPECT_CALL(handler, Invoke(StrEq("add_filesystem"),_)).Times(1)
        .WillOnce(Return("{\"id\": \"test\"}"));
    EXPECT_CALL(handler, Invoke(StrEq("lookup"), _)).Times(AnyNumber())
        .WillRepeatedly(Throw(std::runtime_error("unknown")));
    EXPECT_CALL(handler, Invoke(StrEq("lookup"), Lookup(1, "a.file"))).Times(1)
        .WillOnce(Return("{\"inode\": 2, \"mode\": 420, \"type\": \"file\", \"size\": 4096}"));
    EXPECT_CALL(handler, Invoke(StrEq("getattr"), GetAttr(1))).Times(AnyNumber())
        .WillRepeatedly(Return("{\"mode\": 420, \"type\": \"dir\"}"));
    EXPECT_CALL(handler, Invoke(StrEq("getattr"), GetAttr(2))).Times(AnyNumber())
        .WillRepeatedly(Return("{\"mode\": 420, \"type\": \"file\", \"size\": 4096}"));
    EXPECT_CALL(handler, Invoke(StrEq("open"), Open(2))).Times(1)
        .WillOnce(Return("{\"handle\": 42}"));
    EXPECT_CALL(handler, Invoke(StrEq("read"), _)).Times(AnyNumber())
        .WillRepeatedly(Invoke([](char const *, json_t * params) {
            int offset = json_integer_value(json_array_get(params, 3));
            int length = json_integer_value(json_array_get(params, 4));

            int remaining = (offset < 4096) ? 4096 - offset : 0;
            int count = (length < remaining) ? length : remaining;

            std::string data = std::string(count, '*');

            json_t * result = json_object();
            json_object_set_new(result, "data", json_string(data.c_str()));
            json_object_set_new(result, "format", json_string("identity"));
            json_object_set_new(result, "count", json_integer(count));

            char * result_text = json_dumps(result, 0);
            std::string result_str = result_text;
            free(result_text);
            json_decref(result);

            return result_str;
        })); 
    EXPECT_CALL(handler, Invoke(StrEq("close"), _)).Times(AtMost(1));

    MockAdapterClientCallback callback;
    EXPECT_CALL(callback, Invoke(_, _, _)).Times(AnyNumber());

    std::promise<void> connected;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { connected.set_value(); }));

    std::promise<void> disconnected;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_DISCONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { disconnected.set_value(); }));

    std::promise<void> called;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_FILESYSTEM_ADDED, nullptr)).Times(1)
        .WillOnce(Invoke([&called] (wf_client *, int, void *) mutable {
            called.set_value();
        }));

    AdapterClient client(callback.GetCallbackFn(), callback.GetUserData(), server.GetUrl());

    client.Connect();
    ASSERT_EQ(std::future_status::ready, connected.get_future().wait_for(TIMEOUT));

    client.AddFileSystem();
    ASSERT_EQ(std::future_status::ready, called.get_future().wait_for(TIMEOUT));

    std::string base_dir = client.GetDir();
    ASSERT_TRUE(File(base_dir).isDirectory());
    File file(base_dir + "/a.file");
    std::string contents(4096, '*');
    ASSERT_TRUE(file.hasContents(contents));

    client.Disconnect();
    ASSERT_EQ(std::future_status::ready, disconnected.get_future().wait_for(TIMEOUT));
}

TEST(AdapterClient, ReadLargeFile)
{
    MockInvokationHander handler;
    WsServer server(handler, WF_PROTOCOL_NAME_PROVIDER_SERVER);
    EXPECT_CALL(handler, Invoke(StrEq("add_filesystem"),_)).Times(1)
        .WillOnce(Return("{\"id\": \"test\"}"));
    EXPECT_CALL(handler, Invoke(StrEq("lookup"), _)).Times(AnyNumber())
        .WillRepeatedly(Throw(std::runtime_error("unknown")));
    EXPECT_CALL(handler, Invoke(StrEq("lookup"), Lookup(1, "a.file"))).Times(1)
        .WillOnce(Return("{\"inode\": 2, \"mode\": 420, \"type\": \"file\", \"size\": 102400}"));
    EXPECT_CALL(handler, Invoke(StrEq("getattr"), GetAttr(1))).Times(AnyNumber())
        .WillRepeatedly(Return("{\"mode\": 420, \"type\": \"dir\"}"));
    EXPECT_CALL(handler, Invoke(StrEq("getattr"), GetAttr(2))).Times(AnyNumber())
        .WillRepeatedly(Return("{\"mode\": 420, \"type\": \"file\", \"size\": 102400}"));
    EXPECT_CALL(handler, Invoke(StrEq("open"), Open(2))).Times(1)
        .WillOnce(Return("{\"handle\": 42}"));
    EXPECT_CALL(handler, Invoke(StrEq("read"), _)).Times(AnyNumber())
        .WillRepeatedly(Invoke([](char const *, json_t * params) {
            int offset = json_integer_value(json_array_get(params, 3));
            int length = json_integer_value(json_array_get(params, 4));

            int remaining = (offset < 102400) ? 102400 - offset : 0;
            int count = (length < remaining) ? length : remaining;

            std::string data = std::string(count, '*');

            json_t * result = json_object();
            json_object_set_new(result, "data", json_string(data.c_str()));
            json_object_set_new(result, "format", json_string("identity"));
            json_object_set_new(result, "count", json_integer(count));

            char * result_text = json_dumps(result, 0);
            std::string result_str = result_text;
            free(result_text);
            json_decref(result);

            return result_str;
        })); 
    EXPECT_CALL(handler, Invoke(StrEq("close"), _)).Times(AtMost(1));

    MockAdapterClientCallback callback;
    EXPECT_CALL(callback, Invoke(_, _, _)).Times(AnyNumber());

    std::promise<void> connected;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { connected.set_value(); }));

    std::promise<void> disconnected;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_DISCONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { disconnected.set_value(); }));

    std::promise<void> called;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_FILESYSTEM_ADDED, nullptr)).Times(1)
        .WillOnce(Invoke([&called] (wf_client *, int, void *) mutable {
            called.set_value();
        }));

    AdapterClient client(callback.GetCallbackFn(), callback.GetUserData(), server.GetUrl());

    client.Connect();
    ASSERT_EQ(std::future_status::ready, connected.get_future().wait_for(TIMEOUT));

    client.AddFileSystem();
    ASSERT_EQ(std::future_status::ready, called.get_future().wait_for(TIMEOUT));

    std::string base_dir = client.GetDir();
    ASSERT_TRUE(File(base_dir).isDirectory());
    File file(base_dir + "/a.file");
    ASSERT_TRUE(file.readAll());

    client.Disconnect();
    ASSERT_EQ(std::future_status::ready, disconnected.get_future().wait_for(TIMEOUT));
}