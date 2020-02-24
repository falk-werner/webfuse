#include <gtest/gtest.h>

#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "webfuse/adapter/server.h"
#include "webfuse/adapter/server_config.h"


TEST(server, create_dispose)
{
    mkdir("test", 0700);

    struct wf_server_config * config = wf_server_config_create();
    wf_server_config_set_mountpoint(config, "test");
    struct wf_server * server = wf_server_create(config);
    ASSERT_NE(nullptr, server);

    wf_server_dispose(server);
    wf_server_config_dispose(config);

    rmdir("test");
}