#include <gtest/gtest.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "tempdir.hpp"
#include "webfuse_adapter.h"
#include "webfuse/adapter/impl/uuid_mountpoint.h"

#include <string>

using webfuse_test::TempDir;

namespace
{
    bool is_dir(std::string const & path)
    {
        struct stat info;
        int rc = stat(path.c_str(), &info);
        
        return (0 == rc) && (S_ISDIR(info.st_mode));
    }

    bool is_symlink(std::string const & path)
    {
        struct stat info;
        int rc = lstat(path.c_str(), &info);
        
        return (0 == rc) && (S_ISLNK(info.st_mode));
    }

    bool is_same_path(std::string const & path, std::string const & other)
    {
        struct stat info;
        int rc = stat(path.c_str(), &info);

        struct stat info_other;
        int rc_other = stat(other.c_str(), &info_other);

        return (0 == rc) && (0 == rc_other) && (info.st_ino == info_other.st_ino);
    }
}

TEST(uuid_mountpoint, create)
{
    TempDir temp("uuid_mountpoint");

    std::string filesystem_path = std::string(temp.path()) + "/dummy";
    std::string default_path = std::string(temp.path()) + "/dummy/default";

    wf_mountpoint * mountpoint = wf_impl_uuid_mountpoint_create(temp.path(), "dummy");
    std::string path = wf_mountpoint_get_path(mountpoint);
    ASSERT_NE(nullptr, mountpoint);
    ASSERT_TRUE(is_dir(filesystem_path));
    ASSERT_TRUE(is_symlink(default_path));
    ASSERT_TRUE(is_dir(default_path));
    ASSERT_TRUE(is_dir(path));
    ASSERT_TRUE(is_same_path(default_path, path));

    wf_mountpoint_dispose(mountpoint);
    ASSERT_FALSE(is_dir(filesystem_path));
    ASSERT_FALSE(is_symlink(default_path));
    ASSERT_FALSE(is_dir(default_path));
    ASSERT_FALSE(is_dir(path));
}

TEST(uuid_mountpoint, relink_default)
{
    TempDir temp("uuid_mountpoint");

    std::string filesystem_path = std::string(temp.path()) + "/dummy";
    std::string default_path = std::string(temp.path()) + "/dummy/default";

    wf_mountpoint * mountpoint_a = wf_impl_uuid_mountpoint_create(temp.path(), "dummy");
    std::string path_a = wf_mountpoint_get_path(mountpoint_a);

    wf_mountpoint * mountpoint_b = wf_impl_uuid_mountpoint_create(temp.path(), "dummy");
    std::string path_b = wf_mountpoint_get_path(mountpoint_b);

    ASSERT_TRUE(is_dir(filesystem_path));
    ASSERT_TRUE(is_symlink(default_path));
    ASSERT_TRUE(is_dir(default_path));
    ASSERT_TRUE(is_dir(path_a));
    ASSERT_TRUE(is_dir(path_b));
    ASSERT_TRUE(is_same_path(default_path, path_a));

    wf_mountpoint_dispose(mountpoint_a);
    ASSERT_TRUE(is_dir(filesystem_path));
    ASSERT_TRUE(is_symlink(default_path));
    ASSERT_TRUE(is_dir(default_path));
    ASSERT_FALSE(is_dir(path_a));
    ASSERT_TRUE(is_dir(path_b));
    ASSERT_TRUE(is_same_path(default_path, path_b));

    wf_mountpoint_dispose(mountpoint_b);
    ASSERT_FALSE(is_dir(filesystem_path));
    ASSERT_FALSE(is_symlink(default_path));
    ASSERT_FALSE(is_dir(default_path));
    ASSERT_FALSE(is_dir(path_a));
    ASSERT_FALSE(is_dir(path_b));
}
