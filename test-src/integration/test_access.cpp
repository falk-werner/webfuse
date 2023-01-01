#include "webfuse/webfuse.hpp"
#include "webfuse/test/fixture.hpp"
#include "webfuse/test/filesystem_mock.hpp"
#include "webfuse/test/daemon.hpp"

#include <gtest/gtest.h>
#include <unistd.h>

using testing::_;
using testing::Return;
using testing::Invoke;
using testing::AnyNumber;

namespace
{
int fs_getattr (std::string const & path, struct stat * attr)
{
    memset(reinterpret_cast<void*>(attr),0, sizeof(struct stat));

    if (path == "/")
    {
        attr->st_nlink = 0;
        attr->st_mode = S_IFDIR | 0755;
        return 0;
    }
    else if (path == "/foo")
    {
        attr->st_nlink = 0;
        attr->st_mode = S_IFREG | 0755;
        return 0;
    }
    else
    {
        return -ENOENT;
    }
}
}

TEST(readdir, existing_file)
{
    webfuse::filesystem_mock fs;
    EXPECT_CALL(fs, access("/",_)).Times(AnyNumber()).WillRepeatedly(Return(0));
    EXPECT_CALL(fs, access("/foo", F_OK)).WillOnce(Return(0));
    EXPECT_CALL(fs, access("/foo", R_OK)).WillOnce(Return(0));
    EXPECT_CALL(fs, access("/foo", W_OK)).WillOnce(Return(0));
    EXPECT_CALL(fs, access("/foo", X_OK)).WillOnce(Return(-EACCES));
    EXPECT_CALL(fs, getattr(_,_)).WillRepeatedly(Invoke(fs_getattr));

    webfuse::fixture fixture(fs);
    auto const path = fixture.get_path() + "/foo";

    ASSERT_EQ(0, ::access(path.c_str(), F_OK));
    ASSERT_EQ(0, ::access(path.c_str(), R_OK));
    ASSERT_EQ(0, ::access(path.c_str(), W_OK));
    ASSERT_EQ(-1, ::access(path.c_str(), X_OK));
    ASSERT_EQ(EACCES, errno);
}

TEST(access, non_existing_file)
{
    webfuse::filesystem_mock fs;
    EXPECT_CALL(fs, access("/",_)).Times(AnyNumber()).WillRepeatedly(Return(0));
    EXPECT_CALL(fs, access("/foo", F_OK)).WillOnce(Return(-ENOENT));
    EXPECT_CALL(fs, getattr(_,_)).WillRepeatedly(Invoke(fs_getattr));

    webfuse::fixture fixture(fs);
    auto const path = fixture.get_path() + "/foo";

    ASSERT_EQ(-1, ::access(path.c_str(), F_OK));
    ASSERT_EQ(ENOENT, errno);
}
