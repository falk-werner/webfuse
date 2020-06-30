#include <gtest/gtest.h>

#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <chrono>

#include "webfuse/server.h"
#include "webfuse/server_config.h"
#include "webfuse/test_util/server.hpp"
#include "webfuse/test_util/ws_client.hpp"
#include "webfuse/mocks/mock_invokation_handler.hpp"
#include "webfuse/protocol_names.h"

using webfuse_test::MockInvokationHander;
using webfuse_test::WsClient;
using webfuse_test::Server;

#define TIMEOUT (std::chrono::seconds(10))

namespace
{
struct wf_mountpoint *
create_mountpoint(
    char const * filesystem,
    void * user_data)
{
    (void) filesystem;
    (void) user_data;

    return nullptr;
}

}

TEST(server, create_dispose)
{
    mkdir("test", 0700);

    struct wf_server_config * config = wf_server_config_create();
    wf_server_config_set_mountpoint_factory(config, &create_mountpoint, nullptr);
    struct wf_server * server = wf_server_create(config);
    ASSERT_NE(nullptr, server);

    wf_server_dispose(server);
    wf_server_config_dispose(config);

    rmdir("test");
}

TEST(server, connect)
{
    Server server;
    MockInvokationHander handler;
    WsClient client(handler, WF_PROTOCOL_NAME_PROVIDER_CLIENT);

    auto future = client.Connect(server.GetPort(), WF_PROTOCOL_NAME_ADAPTER_SERVER);
    ASSERT_EQ(std::future_status::ready, future.wait_for(TIMEOUT));
    // future.get();
}