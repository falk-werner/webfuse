#include "webfuse/webfuse.hpp"
#include "webfuse/test/fixture.hpp"
#include "webfuse/test/filesystem_mock.hpp"

#include <gtest/gtest.h>

#include <fcntl.h>

using testing::_;
using testing::Return;
using testing::Invoke;
using testing::AnyNumber;
using testing::AtMost;

TEST(fsync, success)
{
    bool file_created = false;

    webfuse::filesystem_mock fs;
    EXPECT_CALL(fs, access("/",_)).Times(AnyNumber()).WillRepeatedly(Return(0));
    EXPECT_CALL(fs, getattr(_,_)).WillRepeatedly(Invoke([&file_created](std::string const & path, struct stat * attr){
        memset(reinterpret_cast<void*>(attr),0, sizeof(struct stat));

        if (path == "/")
        {
            attr->st_nlink = 1;
            attr->st_mode = S_IFDIR | 0755;
            return 0;
        }
        if ((path == "/some_file") && (file_created))
        {
            attr->st_nlink = 1;
            attr->st_mode = S_IFREG | 0644;
            attr->st_size = 0;
            return 0;
        }
        else
        {
            return -ENOENT;
        }
    }));
    EXPECT_CALL(fs, create("/some_file", _, _)).WillOnce(Invoke([&file_created](auto const &, auto const, auto &){
        file_created = true;
        return 0;
    }));
    EXPECT_CALL(fs, fsync("/some_file", _, _)).WillOnce(Return(0));
    EXPECT_CALL(fs, release("/some_file", _)).Times(AtMost(1)).WillOnce(Return(0));

    webfuse::fixture fixture(fs);
    auto const path = fixture.get_path() + "/some_file";

    int fd = creat(path.c_str(), 0644);
    ASSERT_LT(0, fd);
    ASSERT_EQ(0, ::fsync(fd));
    close(fd);
}
