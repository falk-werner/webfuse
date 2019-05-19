#include <gtest/gtest.h>
#include "integration/server.hpp"
#include "integration/provider.hpp"

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
#include "die_if.hpp"

using webfuse_test::Server;
using webfuse_test::Provider;
using webfuse_test::die_if;

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
                provider = new Provider("ws://localhost:8080/");
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
    std::string file_name = std::string(GetBaseDir()) + "/cprovider/default/hello.txt";

    ASSERT_EXIT({
        struct stat buffer;
        int rc = stat(file_name.c_str(), &buffer);

        die_if(0 != rc);
        die_if(!S_ISREG(buffer.st_mode));
        die_if(0444 != (buffer.st_mode & 0777));
        die_if(12 != buffer.st_size);

        exit(0);
    }, ::testing::ExitedWithCode(0), ".*");      
}

TEST_F(IntegrationTest, ReadTextFile)
{
    std::string file_name = std::string(GetBaseDir()) + "/cprovider/default/hello.txt";

    ASSERT_EXIT({
        FILE * file = fopen(file_name.c_str(), "rb");
        die_if(nullptr == file);

        char buffer[13];
        ssize_t count = fread(buffer, 1, 12, file);
        int rc = fclose(file);

        die_if(12 != count);
        die_if(0 != strncmp("Hello, World", buffer, 12));
        die_if(0 != rc);

        exit(0);
    }, ::testing::ExitedWithCode(0), ".*");      

}

TEST_F(IntegrationTest, ReadDir)
{    
    std::string dir_name = std::string(GetBaseDir()) + "/cprovider/default";

    ASSERT_EXIT({

        DIR * dir = opendir(dir_name.c_str());
        die_if(nullptr == dir);

        bool found_self = false;
        bool found_parent = false;
        bool found_hello_txt = false;
        bool found_other = false;

        dirent * entry = readdir(dir);
        while (NULL != entry)
        {
            if (0 == strcmp(".", entry->d_name))
            {
                found_self = true;
            }
            else if (0 == strcmp("..", entry->d_name))
            {
                found_parent = true;
            }
            else if (0 == strcmp("hello.txt", entry->d_name))
            {
                found_hello_txt = true;
            }
            else
            {
                found_other = true;
            }
            

            entry = readdir(dir);
        }

        closedir(dir);

        die_if(!found_self);
        die_if(!found_parent);
        die_if(!found_hello_txt);

        die_if(found_other);

        exit(0);
    }, ::testing::ExitedWithCode(0), ".*");      

}