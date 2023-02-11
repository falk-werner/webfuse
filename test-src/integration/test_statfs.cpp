#include "webfuse/webfuse.hpp"
#include "webfuse/test/fixture.hpp"
#include "webfuse/test/filesystem_mock.hpp"

#include <gtest/gtest.h>

#include <sys/statvfs.h>

#include <fstream>
#include <sstream>


using testing::_;
using testing::Return;
using testing::Invoke;
using testing::AnyNumber;
using testing::AtMost;

TEST(statfs, success)
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
    EXPECT_CALL(fs, statfs("/", _)).Times(AnyNumber()).WillRepeatedly(Invoke([](auto const &, struct statvfs * statistics){
        return 0;
    }));

    webfuse::fixture fixture(fs);
    auto const path = fixture.get_path() + "/";

    struct statvfs statistics;
    ASSERT_EQ(0, ::statvfs(path.c_str(), &statistics));
}
