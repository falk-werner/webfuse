#include "webfuse/webfuse.hpp"
#include "webfuse/test/fixture.hpp"
#include "webfuse/test/filesystem_mock.hpp"

#include <gtest/gtest.h>
#include <fcntl.h>
#include <sys/sysmacros.h>

using testing::_;
using testing::Return;
using testing::Invoke;
using testing::AnyNumber;
using testing::AtMost;

TEST(mknod, success)
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
        else if ((path == "/some_dev") && (file_created))
        {
            attr->st_nlink = 1;
            attr->st_mode = S_IFIFO | 0644;
            attr->st_size = 0;
            return 0;
        }
        else
        {
            return -ENOENT;
        }
    }));
    EXPECT_CALL(fs, mknod("/some_dev", _, _)).Times(AtMost(1)).WillOnce(Invoke([&file_created](auto const &, auto, auto ){
        file_created = true;
        return 0;
    }));
    EXPECT_CALL(fs, release("/some_dev", _)).Times(AtMost(1)).WillOnce(Return(0));

    webfuse::fixture fixture(fs);
    auto const path = fixture.get_path() + "/some_dev";

    ASSERT_EQ(0, ::mkfifo(path.c_str(), 0644));
}
