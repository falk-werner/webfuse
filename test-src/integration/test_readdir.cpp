#include "webfuse/webfuse.hpp"
#include "webfuse/test/fixture.hpp"
#include "webfuse/test/filesystem_mock.hpp"
#include "webfuse/test/daemon.hpp"

#include <gtest/gtest.h>
#include <unistd.h>
#include <dirent.h>

#include <string>
#include <unordered_map>

using testing::_;
using testing::Return;
using testing::Invoke;
using testing::AnyNumber;

TEST(readdir, existing_dir)
{
    webfuse::filesystem_mock fs;
    EXPECT_CALL(fs, access("/",_)).Times(AnyNumber()).WillRepeatedly(Return(0));
    EXPECT_CALL(fs, getattr(_,_)).WillRepeatedly(Invoke([](auto const & path, auto * attr){
        memset(reinterpret_cast<void*>(attr),0, sizeof(struct stat));

        if ((path == "/") or (path == "/some_dir"))
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
    EXPECT_CALL(fs, readdir("/some_dir",_)).WillOnce(Invoke([](auto const & path, auto & entries) {
        entries.push_back("foo");
        return 0;
    }));

    webfuse::fixture fixture(fs);
    auto const path = fixture.get_path() + "/some_dir";

    std::unordered_map<std::string, bool> expected_entries = {
        {".", false},
        {"..", false},
        {"foo", false},
    };

    DIR * dir = opendir(path.c_str());
    ASSERT_NE(nullptr, dir);
    dirent * entry = readdir(dir);
    int count = 0;
    while (nullptr != entry)
    {
        count++;

        auto it = expected_entries.find(entry->d_name);
        ASSERT_NE(expected_entries.end(), it);
        ASSERT_FALSE(it->second);
        it->second = true;

        entry = readdir(dir);
    }
    closedir(dir);

    ASSERT_EQ(3, count);
}

TEST(readdir, non_existing_dir)
{
    webfuse::filesystem_mock fs;
    EXPECT_CALL(fs, access("/",_)).Times(AnyNumber()).WillRepeatedly(Return(0));
    EXPECT_CALL(fs, getattr(_,_)).WillRepeatedly(Invoke([](auto const & path, auto * attr){
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
    EXPECT_CALL(fs, readdir("/some_dir",_)).Times(0);

    webfuse::fixture fixture(fs);
    auto const path = fixture.get_path() + "/some_dir";

    DIR * dir = opendir(path.c_str());
    ASSERT_EQ(nullptr, dir);
    ASSERT_EQ(ENOENT, errno);
}

