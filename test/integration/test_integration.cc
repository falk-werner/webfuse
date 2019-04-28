#include <gtest/gtest.h>
#include "integration/server.hpp"
#include "integration/provider.hpp"

#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using webfuse_test::Server;
using webfuse_test::Provider;

namespace
{
    class IntegrationTest: public ::testing::Test
    {
        public:
            IntegrationTest()
            : provider("ws://localhost:8080/")
            {

            }
        protected:
            void SetUp()
            {
                server.Start();
                provider.Start();
            }

            void TearDown()
            {
                provider.Stop();
                server.Stop();
            }

            char const * GetBaseDir() const
            {
                return server.GetBaseDir();
            }
        private:
            Server server;
            Provider provider;
    };
}

TEST_F(IntegrationTest, DISABLED_HasMountpoint)
{
    struct stat buffer;
    int rc = stat(GetBaseDir(), &buffer);

    ASSERT_EQ(0, rc);
    ASSERT_TRUE(S_ISDIR(buffer.st_mode));
}

TEST_F(IntegrationTest, DISABLED_ProvidesTextFile)
{
    std::string file_name = std::string(GetBaseDir()) + "/cprovider/default/hello.txt";

    struct stat buffer;
    int rc = stat(file_name.c_str(), &buffer);

    ASSERT_EQ(0, rc);
    // ASSERT_TRUE(S_ISREG(buffer.st_mode));
    // ASSERT_EQ(0444, (buffer.st_mode & 0777));
    // ASSERT_EQ(12, buffer.st_size);
}