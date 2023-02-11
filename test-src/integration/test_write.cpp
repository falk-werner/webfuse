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

TEST(write, success)
{
    webfuse::filesystem_mock fs;
    EXPECT_CALL(fs, access("/",_)).Times(AnyNumber()).WillRepeatedly(Return(0));
    EXPECT_CALL(fs, getattr(_,_)).WillRepeatedly(Invoke([](std::string const & path, struct stat * attr){
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
            return 0;
        }
        else
        {
            return -ENOENT;
        }
    }));
    EXPECT_CALL(fs, open("/some_file", _, _)).WillOnce(Return(0));
    EXPECT_CALL(fs, write("/some_file", _, _, _, _)).Times(AnyNumber()).WillRepeatedly(Invoke([](auto const &, char const * buffer, size_t buffer_size, uint64_t offset, auto){
        return buffer_size;
    }));
    EXPECT_CALL(fs, release("/some_file", _)).Times(AtMost(1)).WillOnce(Return(0));

    webfuse::fixture fixture(fs);
    auto const path = fixture.get_path() + "/some_file";

    {
        std::ofstream out(path);
        out << "Test";
    }
}
