#include "webfuse_adapter.h"
#include "webfuse_provider.h"
#include <libwebsockets.h>

#include "webfuse/utils/tempdir.hpp"

#include <gtest/gtest.h>

using ::webfuse_test::TempDir;

extern "C"
{

wf_mountpoint *
wf_test_integration_lowlevel_create_mountpoint(
    char const *, void * user_data)
{
    auto * tempDir = reinterpret_cast<TempDir*>(user_data);
    return wf_mountpoint_create(tempDir->path());
}

void
wf_test_integration_lowlevel_on_connected(
    void * user_data)
{
    int * state = reinterpret_cast<int*>(user_data);
    *state = 1;
}

void
wf_test_integration_lowlevel_on_disconnected(
    void * user_data)
{
    int * state = reinterpret_cast<int*>(user_data);
    *state = -1;
}

bool
wf_test_integration_lowlevel_authenticate(
    struct wf_credentials const * credentials,
    void * )
{
    char const * username = wf_credentials_get(credentials, "username");
    char const * password = wf_credentials_get(credentials, "password");

    return ((0 == strcmp(username, "bob")) && (0 == strcmp(password, "secret")));
}

void
wf_test_integration_lowlevel_get_credentials(
    struct wfp_credentials * credentials,
    void * )
{
    wfp_credentials_set_type(credentials, "username");
    wfp_credentials_add(credentials, "username", "bob");
    wfp_credentials_add(credentials, "password", "secret");
}

}

TEST(integration, lowlevel)
{
    TempDir dir("wf_test");

    wf_server_protocol * server_protocol = wf_server_protocol_create(
        &wf_test_integration_lowlevel_create_mountpoint, 
        reinterpret_cast<void*>(&dir));
    ASSERT_NE(nullptr, server_protocol);
    wf_server_protocol_add_authenticator(server_protocol, "username",
        &wf_test_integration_lowlevel_authenticate, nullptr);

    int state = 0;
    wfp_client_config * client_config = wfp_client_config_create();
    ASSERT_NE(nullptr, client_config);
    wfp_client_config_set_userdata(client_config, reinterpret_cast<void*>(&state));
    wfp_client_config_set_onconnected(client_config, &wf_test_integration_lowlevel_on_connected);
    wfp_client_config_set_ondisconnected(client_config, &wf_test_integration_lowlevel_on_disconnected);
    wfp_client_config_enable_authentication(client_config, &wf_test_integration_lowlevel_get_credentials);

    wfp_client_protocol * client_protocol = wfp_client_protocol_create(client_config);
    ASSERT_NE(nullptr, client_protocol);

    lws_protocols protocols[3];
    memset(protocols, 0, 3 * sizeof(lws_protocols));
    wf_server_protocol_init_lws(server_protocol, &protocols[0]);
    wfp_client_protocol_init_lws(client_protocol, &protocols[1]);

    lws_context_creation_info info;
    memset(&info, 0, sizeof(info));
    info.port = 8080;
    info.protocols = protocols;
    info.vhost_name = "localhost";
    info.ws_ping_pong_interval = 10;
    info.options = LWS_SERVER_OPTION_HTTP_HEADERS_SECURITY_BEST_PRACTICES_ENFORCE;

    struct lws_context * context = lws_create_context(&info);
    ASSERT_NE(nullptr, context);

    wfp_client_protocol_connect(client_protocol, context, "ws://localhost:8080/");
    while (0 == state)
    {
        lws_service(context, 0);
    }

    EXPECT_EQ(1, state);

    lws_context_destroy(context);

    wfp_client_protocol_dispose(client_protocol);
    wfp_client_config_dispose(client_config);
    wf_server_protocol_dispose(server_protocol);
}