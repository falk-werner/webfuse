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

TEST(access, ok_NO_MEMCHECK)
{
    webfuse::filesystem_mock fs;
    EXPECT_CALL(fs, access("/",_)).Times(AnyNumber()).WillRepeatedly(Return(0));
    EXPECT_CALL(fs, access("/foo",_)).WillOnce(Return(0));
    EXPECT_CALL(fs, getattr(_,_)).WillRepeatedly(Invoke(fs_getattr));

    webfuse::fixture fixture(fs);
    auto const path = fixture.get_path() + "/foo";

    int const rc = ::access(path.c_str(), F_OK);
    ASSERT_EQ(0, rc);
}
