#include <gtest/gtest.h>
#include "wsfs/server.h"

TEST(server, create_dispose)
{
    wsfs_server_config config = {nullptr, nullptr, nullptr, nullptr, 0};
    wsfs_server * server = wsfs_server_create(&config);
    ASSERT_NE(nullptr, server);

    wsfs_server_dispose(server);
}