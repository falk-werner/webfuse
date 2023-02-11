#include "webfuse/webfuse.hpp"
#include "webfuse/test/fixture.hpp"
#include "webfuse/test/filesystem_mock.hpp"

#include <gtest/gtest.h>

using testing::_;
using testing::Return;
using testing::Invoke;
using testing::AnyNumber;

TEST(chmod, success)
{
    bool link_created = false;

    webfuse::filesystem_mock fs;
    EXPECT_CALL(fs, access("/",_)).Times(AnyNumber()).WillRepeatedly(Return(0));
    EXPECT_CALL(fs, getattr(_,_)).WillRepeatedly(Invoke([&link_created](std::string const & path, struct stat * attr){
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
            attr->st_mode = S_IFREG | 0755;
            return 0;
        }
        else
        {
            return -ENOENT;
        }
    }));
    EXPECT_CALL(fs, chmod("/some_file", S_IFREG | 0644)).WillOnce(Return(0));

    webfuse::fixture fixture(fs);
    auto const path = fixture.get_path() + "/some_file";

    ASSERT_EQ(0, ::chmod(path.c_str(), 0644));
}

TEST(chmod, fail)
{
    bool link_created = false;

    webfuse::filesystem_mock fs;
    EXPECT_CALL(fs, access("/",_)).Times(AnyNumber()).WillRepeatedly(Return(0));
    EXPECT_CALL(fs, getattr(_,_)).WillRepeatedly(Invoke([&link_created](std::string const & path, struct stat * attr){
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
            attr->st_mode = S_IFREG | 0755;
            return 0;
        }
        else
        {
            return -ENOENT;
        }
    }));
    EXPECT_CALL(fs, chmod("/some_file", S_IFREG | 0644)).WillOnce(Return(-EACCES));

    webfuse::fixture fixture(fs);
    auto const path = fixture.get_path() + "/some_file";

    ASSERT_NE(0, ::chmod(path.c_str(), 0644));
    ASSERT_EQ(EACCES, errno);
}
