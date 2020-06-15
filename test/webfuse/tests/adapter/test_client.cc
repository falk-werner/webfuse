#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "webfuse/utils/adapter_client.hpp"
#include "webfuse/adapter/client_tlsconfig.h"
#include "webfuse/adapter/credentials.h"
#include "webfuse/core/protocol_names.h"
#include "webfuse/utils/ws_server2.hpp"
#include "webfuse/mocks/mock_adapter_client_callback.hpp"
#include "webfuse/mocks/mock_invokation_handler.hpp"
#include "webfuse/utils/timeout_watcher.hpp"
#include "webfuse/tests/integration/file.hpp"
#include "webfuse/mocks/lookup_matcher.hpp"

using webfuse_test::AdapterClient;
using webfuse_test::WsServer2;
using webfuse_test::MockInvokationHander;
using webfuse_test::MockAdapterClientCallback;
using webfuse_test::TimeoutWatcher;
using webfuse_test::File;
using webfuse_test::Lookup;
using testing::_;
using testing::Invoke;
using testing::AnyNumber;
using testing::Return;
using testing::Throw;
using testing::StrEq;

#define TIMEOUT (std::chrono::milliseconds(30 * 1000))

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
    TimeoutWatcher watcher(TIMEOUT);

    MockInvokationHander handler;
    WsServer2 server(handler, WF_PROTOCOL_NAME_PROVIDER_SERVER);
    EXPECT_CALL(handler, Invoke(_,_)).Times(0);

    MockAdapterClientCallback callback;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_INIT, nullptr)).Times(1);
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CREATED, nullptr)).Times(1);
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_GET_TLS_CONFIG, _)).Times(1);
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CLEANUP, nullptr)).Times(1);

    bool connected = false;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { connected = true; }));

    bool disconnected = false;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_DISCONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { disconnected = true; }));

    AdapterClient client(callback.GetCallbackFn(), callback.GetUserData(), server.GetUrl());

    client.Connect();
    ASSERT_TRUE(watcher.waitUntil([&]() mutable { return connected; }));

    client.Disconnect();
    ASSERT_TRUE(watcher.waitUntil([&]() mutable { return disconnected; }));
}

TEST(AdapterClient, IgnoreNonJsonMessage)
{
    TimeoutWatcher watcher(TIMEOUT);

    MockInvokationHander handler;
    WsServer2 server(handler, WF_PROTOCOL_NAME_PROVIDER_SERVER);
    EXPECT_CALL(handler, Invoke(_,_)).Times(0);

    MockAdapterClientCallback callback;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_INIT, nullptr)).Times(1);
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CREATED, nullptr)).Times(1);
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_GET_TLS_CONFIG, _)).Times(1);
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CLEANUP, nullptr)).Times(1);

    bool connected = false;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { connected = true; }));

    bool disconnected = false;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_DISCONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { disconnected = true; }));

    AdapterClient client(callback.GetCallbackFn(), callback.GetUserData(), server.GetUrl());

    client.Connect();
    ASSERT_TRUE(watcher.waitUntil([&]() mutable { return connected; }));

    server.SendMessage("brummni");

    client.Disconnect();
    ASSERT_TRUE(watcher.waitUntil([&]() mutable { return disconnected; }));
}


TEST(AdapterClient, IgnoreInvalidJsonMessage)
{
    TimeoutWatcher watcher(TIMEOUT);

    MockInvokationHander handler;
    WsServer2 server(handler, WF_PROTOCOL_NAME_PROVIDER_SERVER);
    EXPECT_CALL(handler, Invoke(_,_)).Times(0);

    MockAdapterClientCallback callback;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_INIT, nullptr)).Times(1);
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CREATED, nullptr)).Times(1);
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_GET_TLS_CONFIG, _)).Times(1);
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CLEANUP, nullptr)).Times(1);

    bool connected = false;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { connected = true; }));

    bool disconnected = false;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_DISCONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { disconnected = true; }));

    AdapterClient client(callback.GetCallbackFn(), callback.GetUserData(), server.GetUrl());

    client.Connect();
    ASSERT_TRUE(watcher.waitUntil([&]() mutable { return connected; }));

    json_t * invalid_request = json_object();
    server.SendMessage(invalid_request);

    client.Disconnect();
    ASSERT_TRUE(watcher.waitUntil([&]() mutable { return disconnected; }));
}

TEST(AdapterClient, ConnectWithTls)
{
    TimeoutWatcher watcher(TIMEOUT);

    MockInvokationHander handler;
    WsServer2 server(handler, WF_PROTOCOL_NAME_PROVIDER_SERVER, 0, true);
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

    bool connected = false;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { connected = true; }));

    bool disconnected = false;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_DISCONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { disconnected = true; }));

    AdapterClient client(callback.GetCallbackFn(), callback.GetUserData(), server.GetUrl());

    client.Connect();
    ASSERT_TRUE(watcher.waitUntil([&]() mutable { return connected; }));

    client.Disconnect();
    ASSERT_TRUE(watcher.waitUntil([&]() mutable { return disconnected; }));
}

TEST(AdapterClient, FailedToConnectInvalidPort)
{
    TimeoutWatcher watcher(TIMEOUT);


    MockAdapterClientCallback callback;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_INIT, nullptr)).Times(1);
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CREATED, nullptr)).Times(1);
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_GET_TLS_CONFIG, _)).Times(1);
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CLEANUP, nullptr)).Times(1);

    bool disconnected = false;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_DISCONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { disconnected = true; }));

    AdapterClient client(callback.GetCallbackFn(), callback.GetUserData(), "ws://localhost:4/");

    client.Connect();
    ASSERT_TRUE(watcher.waitUntil([&]() mutable { return disconnected; }));
}


TEST(AdapterClient, Authenticate)
{
    TimeoutWatcher watcher(TIMEOUT);

    MockInvokationHander handler;
    WsServer2 server(handler, WF_PROTOCOL_NAME_PROVIDER_SERVER);
    EXPECT_CALL(handler, Invoke(StrEq("authenticate"),_)).Times(1)
        .WillOnce(Return("{}"));

    MockAdapterClientCallback callback;
    EXPECT_CALL(callback, Invoke(_, _, _)).Times(AnyNumber());

    bool connected = false;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { connected = true; }));

    bool disconnected = false;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_DISCONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { disconnected = true; }));

    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_AUTHENTICATE_GET_CREDENTIALS, _)).Times(1)
        .WillOnce(Invoke(GetCredentials));

    bool authenticated = false;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_AUTHENTICATED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { authenticated = true; }));

    AdapterClient client(callback.GetCallbackFn(), callback.GetUserData(), server.GetUrl());

    client.Connect();
    ASSERT_TRUE(watcher.waitUntil([&]() mutable { return connected; }));

    client.Authenticate();
    ASSERT_TRUE(watcher.waitUntil([&]() mutable { return authenticated; }));

    client.Disconnect();
    ASSERT_TRUE(watcher.waitUntil([&]() mutable { return disconnected; }));
}

TEST(AdapterClient, AuthenticateFailedWithoutConnect)
{
    TimeoutWatcher watcher(TIMEOUT);

    MockAdapterClientCallback callback;
    EXPECT_CALL(callback, Invoke(_, _, _)).Times(AnyNumber());
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_AUTHENTICATE_GET_CREDENTIALS, _)).Times(1)
        .WillOnce(Invoke(GetCredentials));
    bool called = false;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_AUTHENTICATION_FAILED, nullptr)).Times(1)
        .WillOnce(Invoke([&called] (wf_client *, int, void *) mutable {
            called = true;
        }));

    AdapterClient client(callback.GetCallbackFn(), callback.GetUserData(), "");

    client.Authenticate();
    ASSERT_TRUE(watcher.waitUntil([&]() mutable { return called; }));
}

TEST(AdapterClient, AuthenticationFailed)
{
    TimeoutWatcher watcher(TIMEOUT);

    MockInvokationHander handler;
    WsServer2 server(handler, WF_PROTOCOL_NAME_PROVIDER_SERVER);
    EXPECT_CALL(handler, Invoke(StrEq("authenticate"),_)).Times(1)
        .WillOnce(Throw(std::runtime_error("authentication failed")));

    MockAdapterClientCallback callback;
    EXPECT_CALL(callback, Invoke(_, _, _)).Times(AnyNumber());

    bool connected = false;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { connected = true; }));

    bool disconnected = false;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_DISCONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { disconnected = true; }));

    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_AUTHENTICATE_GET_CREDENTIALS, _)).Times(1)
        .WillOnce(Invoke(GetCredentials));

    bool called = false;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_AUTHENTICATION_FAILED, nullptr)).Times(1)
        .WillOnce(Invoke([&called] (wf_client *, int, void *) mutable {
            called = true;
        }));

    AdapterClient client(callback.GetCallbackFn(), callback.GetUserData(), server.GetUrl());

    client.Connect();
    ASSERT_TRUE(watcher.waitUntil([&]() mutable { return connected; }));

    client.Authenticate();
    ASSERT_TRUE(watcher.waitUntil([&]() mutable { return called; }));

    client.Disconnect();
    ASSERT_TRUE(watcher.waitUntil([&]() mutable { return disconnected; }));
}

TEST(AdapterClient, AddFileSystem)
{
    TimeoutWatcher watcher(TIMEOUT);

    MockInvokationHander handler;
    WsServer2 server(handler, WF_PROTOCOL_NAME_PROVIDER_SERVER);
    EXPECT_CALL(handler, Invoke(StrEq("add_filesystem"),_)).Times(1)
        .WillOnce(Return("{\"id\": \"test\"}"));
    EXPECT_CALL(handler, Invoke(StrEq("lookup"), _)).Times(AnyNumber())
        .WillRepeatedly(Throw(std::runtime_error("unknown")));

    MockAdapterClientCallback callback;
    EXPECT_CALL(callback, Invoke(_, _, _)).Times(AnyNumber());

    bool connected = false;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { connected = true; }));

    bool disconnected = false;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_DISCONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { disconnected = true; }));

    bool called = false;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_FILESYSTEM_ADDED, nullptr)).Times(1)
        .WillOnce(Invoke([&called] (wf_client *, int, void *) mutable {
            called = true;
        }));

    AdapterClient client(callback.GetCallbackFn(), callback.GetUserData(), server.GetUrl());

    client.Connect();
    ASSERT_TRUE(watcher.waitUntil([&]() mutable { return connected; }));

    client.AddFileSystem();
    ASSERT_TRUE(watcher.waitUntil([&]() mutable { return called; }));

    client.Disconnect();
    ASSERT_TRUE(watcher.waitUntil([&]() mutable { return disconnected; }));
}

TEST(AdapterClient, FailToAddFileSystemTwice)
{
    TimeoutWatcher watcher(TIMEOUT);

    MockInvokationHander handler;
    WsServer2 server(handler, WF_PROTOCOL_NAME_PROVIDER_SERVER);
    EXPECT_CALL(handler, Invoke(StrEq("add_filesystem"),_)).Times(1)
        .WillOnce(Return("{\"id\": \"test\"}"));
    EXPECT_CALL(handler, Invoke(StrEq("lookup"), _)).Times(AnyNumber())
        .WillRepeatedly(Throw(std::runtime_error("unknown")));

    MockAdapterClientCallback callback;
    EXPECT_CALL(callback, Invoke(_, _, _)).Times(AnyNumber());

    bool connected = false;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { connected = true; }));

    bool disconnected = false;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_DISCONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { disconnected = true; }));

    bool filesystem_added = false;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_FILESYSTEM_ADDED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable {
            filesystem_added = true;
        }));

    bool filesystem_add_failed = false;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_FILESYSTEM_ADD_FAILED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable {
            filesystem_add_failed = true;
        }));

    AdapterClient client(callback.GetCallbackFn(), callback.GetUserData(), server.GetUrl());

    client.Connect();
    ASSERT_TRUE(watcher.waitUntil([&]() mutable { return connected; }));

    client.AddFileSystem();
    ASSERT_TRUE(watcher.waitUntil([&]() mutable { return filesystem_added; }));

    client.AddFileSystem();
    ASSERT_TRUE(watcher.waitUntil([&]() mutable { return filesystem_add_failed; }));

    client.Disconnect();
    ASSERT_TRUE(watcher.waitUntil([&]() mutable { return disconnected; }));
}

TEST(AdapterClient, FailToAddFileSystemMissingId)
{
    TimeoutWatcher watcher(TIMEOUT);

    MockInvokationHander handler;
    WsServer2 server(handler, WF_PROTOCOL_NAME_PROVIDER_SERVER);
    EXPECT_CALL(handler, Invoke(StrEq("add_filesystem"),_)).Times(1)
        .WillOnce(Return("{}"));
    EXPECT_CALL(handler, Invoke(StrEq("lookup"), _)).Times(AnyNumber())
        .WillRepeatedly(Throw(std::runtime_error("unknown")));

    MockAdapterClientCallback callback;
    EXPECT_CALL(callback, Invoke(_, _, _)).Times(AnyNumber());

    bool connected = false;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { connected = true; }));

    bool disconnected = false;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_DISCONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { disconnected = true; }));

    bool filesystem_add_failed = false;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_FILESYSTEM_ADD_FAILED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable {
            filesystem_add_failed = true;
        }));

    AdapterClient client(callback.GetCallbackFn(), callback.GetUserData(), server.GetUrl());

    client.Connect();
    ASSERT_TRUE(watcher.waitUntil([&]() mutable { return connected; }));

    client.AddFileSystem();
    ASSERT_TRUE(watcher.waitUntil([&]() mutable { return filesystem_add_failed; }));

    client.Disconnect();
    ASSERT_TRUE(watcher.waitUntil([&]() mutable { return disconnected; }));
}

TEST(AdapterClient, FailToAddFileSystemIdNotString)
{
    TimeoutWatcher watcher(TIMEOUT);

    MockInvokationHander handler;
    WsServer2 server(handler, WF_PROTOCOL_NAME_PROVIDER_SERVER);
    EXPECT_CALL(handler, Invoke(StrEq("add_filesystem"),_)).Times(1)
        .WillOnce(Return("{\"id\": 42}"));
    EXPECT_CALL(handler, Invoke(StrEq("lookup"), _)).Times(AnyNumber())
        .WillRepeatedly(Throw(std::runtime_error("unknown")));

    MockAdapterClientCallback callback;
    EXPECT_CALL(callback, Invoke(_, _, _)).Times(AnyNumber());

    bool connected = false;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { connected = true; }));

    bool disconnected = false;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_DISCONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { disconnected = true; }));

    bool filesystem_add_failed = false;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_FILESYSTEM_ADD_FAILED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable {
            filesystem_add_failed = true;
        }));

    AdapterClient client(callback.GetCallbackFn(), callback.GetUserData(), server.GetUrl());

    client.Connect();
    ASSERT_TRUE(watcher.waitUntil([&]() mutable { return connected; }));

    client.AddFileSystem();
    ASSERT_TRUE(watcher.waitUntil([&]() mutable { return filesystem_add_failed; }));

    client.Disconnect();
    ASSERT_TRUE(watcher.waitUntil([&]() mutable { return disconnected; }));
}


TEST(AdapterClient, AddFileSystemFailed)
{
    TimeoutWatcher watcher(TIMEOUT);

    MockInvokationHander handler;
    WsServer2 server(handler, WF_PROTOCOL_NAME_PROVIDER_SERVER);
    EXPECT_CALL(handler, Invoke(StrEq("add_filesystem"),_)).Times(1)
        .WillOnce(Throw(std::runtime_error("failed")));

    MockAdapterClientCallback callback;
    EXPECT_CALL(callback, Invoke(_, _, _)).Times(AnyNumber());

    bool connected = false;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { connected = true; }));

    bool disconnected = false;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_DISCONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { disconnected = true; }));

    bool called = false;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_FILESYSTEM_ADD_FAILED, nullptr)).Times(1)
        .WillOnce(Invoke([&called] (wf_client *, int, void *) mutable {
            called = true;
        }));

    AdapterClient client(callback.GetCallbackFn(), callback.GetUserData(), server.GetUrl());

    client.Connect();
    ASSERT_TRUE(watcher.waitUntil([&]() mutable { return connected; }));

    client.AddFileSystem();
    ASSERT_TRUE(watcher.waitUntil([&]() mutable { return called; }));

    client.Disconnect();
    ASSERT_TRUE(watcher.waitUntil([&]() mutable { return disconnected; }));
}

TEST(AdapterClient, LookupFile)
{
    TimeoutWatcher watcher(TIMEOUT);

    MockInvokationHander handler;
    WsServer2 server(handler, WF_PROTOCOL_NAME_PROVIDER_SERVER);
    EXPECT_CALL(handler, Invoke(StrEq("add_filesystem"),_)).Times(1)
        .WillOnce(Return("{\"id\": \"test\"}"));
    EXPECT_CALL(handler, Invoke(StrEq("lookup"), _)).Times(AnyNumber())
        .WillRepeatedly(Throw(std::runtime_error("unknown")));
    EXPECT_CALL(handler, Invoke(StrEq("lookup"), Lookup(1, "Hello.txt"))).Times(AnyNumber())
        .WillRepeatedly(Return(
            "{"
            "\"inode\": 2,"
            "\"mode\": 420,"    //0644
            "\"type\": \"file\","
            "\"size\": 42,"
            "\"atime\": 0,"
            "\"mtime\": 0,"
            "\"ctime\": 0"
            "}"
        ));

    MockAdapterClientCallback callback;
    EXPECT_CALL(callback, Invoke(_, _, _)).Times(AnyNumber());

    bool connected = false;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { connected = true; }));

    bool disconnected = false;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_DISCONNECTED, nullptr)).Times(1)
        .WillOnce(Invoke([&] (wf_client *, int, void *) mutable { disconnected = true; }));

    bool called = false;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_FILESYSTEM_ADDED, nullptr)).Times(1)
        .WillOnce(Invoke([&called] (wf_client *, int, void *) mutable {
            called = true;
        }));

    AdapterClient client(callback.GetCallbackFn(), callback.GetUserData(), server.GetUrl());

    client.Connect();
    ASSERT_TRUE(watcher.waitUntil([&]() mutable { return connected; }));

    client.AddFileSystem();
    ASSERT_TRUE(watcher.waitUntil([&]() mutable { return called; }));

    std::string file_name = client.GetDir() + "/Hello.txt";
    File file(file_name);
    ASSERT_TRUE(file.isFile());

    client.Disconnect();
    ASSERT_TRUE(watcher.waitUntil([&]() mutable { return disconnected; }));
}
