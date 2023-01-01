#include "webfuse/webfuse.hpp"
#include "webfuse/test/fixture.hpp"
#include "webfuse/test/filesystem_mock.hpp"

#include <gtest/gtest.h>


using testing::_;
using testing::Return;
using testing::Invoke;
using testing::AnyNumber;

namespace
{

int fs_getattr(std::string const & path, struct stat * attr){
        memset(reinterpret_cast<void*>(attr),0, sizeof(struct stat));

        if (path == "/")
        {
            attr->st_nlink = 1;
            attr->st_mode = S_IFDIR | 0755;
            return 0;
        }
        else if (path == "/some_link")
        {
            attr->st_nlink = 1;
            attr->st_mode = S_IFLNK | 0755;
            return 0;            
        }
        else
        {
            return -ENOENT;
        }        
    }

}

TEST(readlink, existing_link)
{
    webfuse::filesystem_mock fs;
    EXPECT_CALL(fs, access("/",_)).Times(AnyNumber()).WillRepeatedly(Return(0));
    EXPECT_CALL(fs, getattr(_,_)).WillRepeatedly(Invoke(fs_getattr));
    EXPECT_CALL(fs, readlink("/some_link",_)).WillOnce(Invoke([](auto const & path, auto & out) {
        out = "link-target";
        return 0;
    }));

    webfuse::fixture fixture(fs);
    auto const path = fixture.get_path() + "/some_link";

    char buffer[100];
    auto const length = ::readlink(path.c_str(), buffer, 99);
    ASSERT_LT(0, length);
    buffer[length] = '\0';
    ASSERT_STREQ("link-target", buffer);
}

TEST(readlink, non_existing_link)
{
    webfuse::filesystem_mock fs;
    EXPECT_CALL(fs, access("/",_)).Times(AnyNumber()).WillRepeatedly(Return(0));
    EXPECT_CALL(fs, getattr(_,_)).WillRepeatedly(Invoke(fs_getattr));
    EXPECT_CALL(fs, readlink("/some_link",_)).WillOnce(Invoke([](auto const & path, auto & out) {
        return -ENOENT;
    }));

    webfuse::fixture fixture(fs);
    auto const path = fixture.get_path() + "/some_link";

    char buffer[100];
    ASSERT_GT(0, ::readlink(path.c_str(), buffer, 100));
    ASSERT_EQ(ENOENT, errno);
}
