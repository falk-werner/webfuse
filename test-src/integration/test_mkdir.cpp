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

TEST(mkdir, success)
{
    bool created = false;

    webfuse::filesystem_mock fs;
    EXPECT_CALL(fs, access("/",_)).Times(AnyNumber()).WillRepeatedly(Return(0));
    EXPECT_CALL(fs, getattr(_,_)).WillRepeatedly(Invoke([&created](std::string const & path, struct stat * attr){
        memset(reinterpret_cast<void*>(attr),0, sizeof(struct stat));

        if (path == "/")
        {
            attr->st_nlink = 1;
            attr->st_mode = S_IFDIR | 0755;
            return 0;
        }
        if ((path == "/some_dir") && (created))
        {
            attr->st_nlink = 1;
            attr->st_mode = S_IFDIR | 0644;
            return 0;
        }
        else
        {
            return -ENOENT;
        }
    }));
    EXPECT_CALL(fs, mkdir("/some_dir", _)).WillOnce(Invoke([&created](auto const &, auto) {
        created = true;
        return 0;
    }));

    webfuse::fixture fixture(fs);
    auto const path = fixture.get_path() + "/some_dir";

    ASSERT_EQ(0, ::mkdir(path.c_str(), 0644));
}
