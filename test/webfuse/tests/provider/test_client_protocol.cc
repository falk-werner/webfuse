#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <webfuse/provider/client_protocol.h>
#include <webfuse/provider/client_config.h>
#include "webfuse/fakes/fake_adapter_server.hpp"

#include <cstring>
#include <thread>
#include <atomic>

using webfuse_test::FakeAdapterServer;
using testing::_;

namespace
{

class ClientProtocolTest: public ::testing::Test
{
protected:
    void SetUp()
    {
        server = new FakeAdapterServer(54321);

        config = wfp_client_config_create();
        protocol = wfp_client_protocol_create(config);

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

        context = lws_create_context(&info);
        wfp_client_protocol_connect(protocol, context, "ws://localhost:54321/");

        isShutdownRequested = false;
        thread = std::thread(run, this);
    }

    void TearDown()
    {
        isShutdownRequested = true;
        thread.join();

        lws_context_destroy(context);

        wfp_client_protocol_dispose(protocol);
        wfp_client_config_dispose(config);

        delete server;
    }

    FakeAdapterServer * server;

private:
    static void run(ClientProtocolTest * self)
    {
        while (!self->isShutdownRequested)
        {
            lws_service(self->context, 100);
        }
    }

    wfp_client_config * config;
    wfp_client_protocol * protocol;
    std::thread thread;
    std::atomic<bool> isShutdownRequested;
    lws_context * context;

};


}


TEST_F(ClientProtocolTest, connect)
{
    server->waitForConnection();    
}