#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <webfuse/provider/client_protocol.h>
#include <webfuse/provider/client_config.h>
#include "fake_adapter_server.hpp"

#include <cstring>
#include <thread>
#include <atomic>

using webfuse_test::FakeAdapterServer;
using testing::_;

namespace
{

struct Context
{
    lws_context * context;
    std::atomic<bool> isShutdownRequested;
};

void run(Context * context)
{
    while (!context->isShutdownRequested)
    {
        lws_service(context->context, 100);
    }
}


}


TEST(client_protocol, connect)
{
    FakeAdapterServer server(54321);

    wfp_client_config * config = wfp_client_config_create();
    wfp_client_protocol * protocol = wfp_client_protocol_create(config);

    struct lws_protocols protocols[2];
    memset(protocols, 0, sizeof(struct lws_protocols) * 2);
    protocols[0].name = "fs";
    wfp_client_protocol_init_lws(protocol, &protocols[0]);

    struct lws_context_creation_info info;
    memset(&info, 0, sizeof(struct lws_context_creation_info));
    info.port = CONTEXT_PORT_NO_LISTEN;
    info.protocols = protocols;
    info.uid = -1;
    info.gid = -1;

    struct lws_context * context = lws_create_context(&info);
    wfp_client_protocol_connect(protocol, context, "ws://localhost:54321/");

    Context ctx;
    ctx.context = context;
    ctx.isShutdownRequested = false;
    std::thread client_thread(run, &ctx);

    server.waitForConnection();
    
    ctx.isShutdownRequested = true;
    client_thread.join();

    lws_context_destroy(context);

    wfp_client_protocol_dispose(protocol);
    wfp_client_config_dispose(config);
}