#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "webfuse/utils/adapter_client.hpp"

#include "webfuse/adapter/credentials.h"
#include "webfuse/core/protocol_names.h"
#include "webfuse/utils/ws_server2.hpp"
#include "webfuse/mocks/mock_adapter_client_callback.hpp"
#include "webfuse/mocks/mock_invokation_handler.hpp"
#include "webfuse/utils/timeout_watcher.hpp"

using webfuse_test::AdapterClient;
using webfuse_test::WsServer2;
using webfuse_test::MockInvokationHander;
using webfuse_test::MockAdapterClientCallback;
using webfuse_test::TimeoutWatcher;
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

    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_CONNECTED, nullptr)).Times(1);
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_DISCONNECTED, nullptr)).Times(1);

    AdapterClient client(callback.GetCallbackFn(), callback.GetUserData(), server.GetUrl());

    client.Connect();
    while (!server.IsConnected())
    {
        watcher.check();
    }

    client.Disconnect();
    while (server.IsConnected())
    {
        watcher.check();
    }
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
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_AUTHENTICATE_GET_CREDENTIALS, _)).Times(1)
        .WillOnce(Invoke(GetCredentials));
    bool called = false;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_AUTHENTICATED, nullptr)).Times(1)
        .WillOnce(Invoke([&called] (wf_client *, int, void *) mutable {
            called = true;
        }));

    AdapterClient client(callback.GetCallbackFn(), callback.GetUserData(), server.GetUrl());

    client.Connect();
    while (!server.IsConnected())
    {
        watcher.check();
    }

    client.Authenticate();
    while (!called) {
        watcher.check();
    }

    client.Disconnect();
    while (server.IsConnected())
    {
        watcher.check();
    }
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
    while (!called) {
        watcher.check();
    }
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
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_AUTHENTICATE_GET_CREDENTIALS, _)).Times(1)
        .WillOnce(Invoke(GetCredentials));
    bool called = false;
    EXPECT_CALL(callback, Invoke(_, WF_CLIENT_AUTHENTICATION_FAILED, nullptr)).Times(1)
        .WillOnce(Invoke([&called] (wf_client *, int, void *) mutable {
            called = true;
        }));

    AdapterClient client(callback.GetCallbackFn(), callback.GetUserData(), server.GetUrl());

    client.Connect();
    while (!server.IsConnected())
    {
        watcher.check();
    }

    client.Authenticate();
    while (!called) {
        watcher.check();
    }

    client.Disconnect();
    while (server.IsConnected())
    {
        watcher.check();
    }
}
