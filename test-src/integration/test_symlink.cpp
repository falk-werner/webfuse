#include "webfuse/webfuse.hpp"
#include "webfuse/test/fixture.hpp"
#include "webfuse/test/filesystem_mock.hpp"

#include <gtest/gtest.h>

using testing::_;
using testing::Return;
using testing::Invoke;
using testing::AnyNumber;

TEST(symlink, sucessfully_create_symlink)
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
        else if ((path == "/some_link") && (link_created))
        {
            attr->st_nlink = 1;
            attr->st_mode = S_IFLNK | 0755;
            return 0;
        }
        else
        {
            return -ENOENT;
        }
    }));
    EXPECT_CALL(fs, symlink("link-target", "/some_link")).WillOnce(Invoke([&link_created](auto const &, auto const &){
        link_created = true;
        return 0;
    }));

    webfuse::fixture fixture(fs);
    auto const path = fixture.get_path() + "/some_link";

    ASSERT_EQ(0, ::symlink("link-target", path.c_str()));
}

TEST(symlink, failed_to_create_symlink)
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
        else
        {
            return -ENOENT;
        }
    }));
    EXPECT_CALL(fs, symlink("link-target", "/some_link")).WillOnce(Return(-EDQUOT));

    webfuse::fixture fixture(fs);
    auto const path = fixture.get_path() + "/some_link";

    ASSERT_NE(0, ::symlink("link-target", path.c_str()));
    ASSERT_EQ(EDQUOT, errno);
}
