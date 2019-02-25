#include <gtest/gtest.h>

#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "wsfs/adapter/server.h"
#include "wsfs/adapter/server_config.h"


TEST(server, create_dispose)
{
    mkdir("test", 0700);

    struct wsfs_server_config config = {strdup("test"), nullptr, nullptr, nullptr, nullptr, 0};
    struct wsfs_server * server = wsfs_server_create(&config);
    ASSERT_NE(nullptr, server);

    wsfs_server_dispose(server);
    wsfs_server_config_cleanup(&config);

    rmdir("test");
}