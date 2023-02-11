#include "webfuse/webfuse.hpp"
#include "webfuse/test/fixture.hpp"
#include "webfuse/test/filesystem_mock.hpp"

#include <gtest/gtest.h>

using testing::_;
using testing::Return;
using testing::Invoke;
using testing::AnyNumber;

TEST(rename, success)
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
    EXPECT_CALL(fs, rename("/some_file", "/another_file", _)).WillOnce(Return(0));

    webfuse::fixture fixture(fs);
    auto const old_name = fixture.get_path() + "/some_file";
    auto const new_name = fixture.get_path() + "/another_file";

    ASSERT_EQ(0, ::rename(old_name.c_str(), new_name.c_str()));
}

TEST(rename, fail)
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
    EXPECT_CALL(fs, rename("/some_file", "/another_file", _)).WillOnce(Return(-EDQUOT));

    webfuse::fixture fixture(fs);
    auto const old_name = fixture.get_path() + "/some_file";
    auto const new_name = fixture.get_path() + "/another_file";

    ASSERT_NE(0, ::rename(old_name.c_str(), new_name.c_str()));
    ASSERT_EQ(EDQUOT, errno);
}
