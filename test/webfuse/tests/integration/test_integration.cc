#include <gtest/gtest.h>
#include "webfuse/tests/integration/server.hpp"
#include "webfuse/tests/integration/provider.hpp"
#include "webfuse/tests/integration/file.hpp"

#include <cstdio>
#include <csignal>
#include <cstring>

#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include <jansson.h>
#include "webfuse/core/lws_log.h"

using webfuse_test::Server;
using webfuse_test::Provider;
using webfuse_test::File;

namespace
{
    class IntegrationTest: public ::testing::Test
    {
        public:
            IntegrationTest()
            : server(nullptr)
            , provider(nullptr)
            {
                json_object_seed(0);
                wf_lwslog_disable();
            }

        protected:
            void SetUp()
            {
                server = new Server();
                provider = new Provider(server->GetUrl().c_str());
            }

            void TearDown()
            {
                delete provider;
                delete server;
            }

            char const * GetBaseDir() const
            {
                return server->GetBaseDir();
            }
        private:
            Server * server;
            Provider * provider;
    };
}

TEST_F(IntegrationTest, HasMountpoint)
{
    struct stat buffer;
    int rc = stat(GetBaseDir(), &buffer);

    ASSERT_EQ(0, rc);
    ASSERT_TRUE(S_ISDIR(buffer.st_mode));
}

TEST_F(IntegrationTest, ProvidesTextFile)
{
    std::string file_name = std::string(GetBaseDir()) + "/cprovider/hello.txt";

    File file(file_name);
    ASSERT_TRUE(file.isFile());
    ASSERT_TRUE(file.hasAccessRights(0444));
    ASSERT_TRUE(file.hasSize(12));
}

TEST_F(IntegrationTest, ReadTextFile)
{
    std::string file_name = std::string(GetBaseDir()) + "/cprovider/hello.txt";

    File file(file_name);
    ASSERT_TRUE(file.hasContents("Hello, World"));
}

TEST_F(IntegrationTest, ReadDir)
{    
    std::string dir_name = std::string(GetBaseDir()) + "/cprovider";

    File dir(dir_name);
    ASSERT_TRUE(dir.isDirectory());
    ASSERT_TRUE(dir.hasSubdirectory("."));
    ASSERT_TRUE(dir.hasSubdirectory(".."));
    ASSERT_TRUE(dir.hasSubdirectory("hello.txt"));
    ASSERT_FALSE(dir.hasSubdirectory("other"));
}