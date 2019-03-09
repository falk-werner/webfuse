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

    struct wsfs_server_config * config = wsfs_server_config_create();
    wsfs_server_config_set_mountpoint(config, "test");
    struct wsfs_server * server = wsfs_server_create(config);
    ASSERT_NE(nullptr, server);

    wsfs_server_dispose(server);
    wsfs_server_config_dispose(config);

    rmdir("test");
}