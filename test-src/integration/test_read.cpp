#include "webfuse/webfuse.hpp"
#include "webfuse/test/fixture.hpp"
#include "webfuse/test/filesystem_mock.hpp"

#include <gtest/gtest.h>
#include <fstream>
#include <sstream>


using testing::_;
using testing::Return;
using testing::Invoke;
using testing::AnyNumber;
using testing::AtMost;

TEST(read, success)
{
    std::string expected = "Hello, World!";

    webfuse::filesystem_mock fs;
    EXPECT_CALL(fs, access("/",_)).Times(AnyNumber()).WillRepeatedly(Return(0));
    EXPECT_CALL(fs, getattr(_,_)).WillRepeatedly(Invoke([&expected](std::string const & path, struct stat * attr){
        memset(reinterpret_cast<void*>(attr),0, sizeof(struct stat));

        if (path == "/")
        {
            attr->st_nlink = 1;
            attr->st_mode = S_IFDIR | 0755;
            return 0;
        }
        if (path == "/some_file")
        {
            attr->st_nlink = 1;
            attr->st_mode = S_IFREG | 0644;
            attr->st_size = expected.size();
            return 0;
        }
        else
        {
            return -ENOENT;
        }
    }));
    EXPECT_CALL(fs, open("/some_file", _, _)).WillOnce(Return(0));
    EXPECT_CALL(fs, read("/some_file", _, _, _, _)).Times(AnyNumber()).WillRepeatedly(Invoke([&expected](auto const &, char * buffer, size_t buffer_size, uint64_t offset, auto){
        if (offset < expected.size())
        {
            auto const count = std::min(buffer_size, expected.size() - offset);
            memcpy(reinterpret_cast<void*>(buffer), reinterpret_cast<void const *>(&expected[offset]), count);
            return (int) count;
        }
        else
        {
            return 0;
        }
    }));
    EXPECT_CALL(fs, release("/some_file", _)).Times(AtMost(1)).WillOnce(Return(0));

    webfuse::fixture fixture(fs);
    auto const path = fixture.get_path() + "/some_file";

    std::string contents;
    {
        std::ifstream in(path);
        std::stringstream buffer;
        buffer << in.rdbuf();
        contents = buffer.str();
    }

    ASSERT_EQ(expected, contents);
}
