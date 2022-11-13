#include "webfuse/filesystem/fileattributes.hpp"

#include <gtest/gtest.h>

using webfuse::file_attributes;
using webfuse::user_id;
using webfuse::group_id;
using webfuse::filemode;

TEST(file_attibutes, create_empty)
{
    file_attributes attributes;

    ASSERT_EQ(0, attributes.inode);
    ASSERT_EQ(0, attributes.nlink);
    ASSERT_EQ(0, attributes.mode);
    ASSERT_EQ(user_id::invalid, attributes.uid);
    ASSERT_EQ(group_id::invalid, attributes.gid);
    ASSERT_EQ(0, attributes.rdev);
    ASSERT_EQ(0, attributes.size);
    ASSERT_EQ(0, attributes.blocks);
    ASSERT_EQ(0, attributes.atime.seconds);
    ASSERT_EQ(0, attributes.atime.nsec);
    ASSERT_EQ(0, attributes.mtime.seconds);
    ASSERT_EQ(0, attributes.mtime.nsec);
    ASSERT_EQ(0, attributes.ctime.seconds);
    ASSERT_EQ(0, attributes.ctime.nsec);
}

TEST(file_attibutes, from_stat)
{
    struct stat info;
    info.st_ino = 1;
    info.st_nlink = 2;
    info.st_mode = S_IFREG | 0644;
    info.st_uid = 1000;
    info.st_gid = 1234;
    info.st_rdev = 0;
    info.st_size = 21 * 1024;
    info.st_blocks = 42;
    info.st_atim.tv_sec = 1;
    info.st_atim.tv_nsec = 2;
    info.st_mtim.tv_sec = 3;
    info.st_mtim.tv_nsec = 4;
    info.st_ctim.tv_sec = 5;
    info.st_ctim.tv_nsec = 6;

    file_attributes attributes(info);

    ASSERT_EQ(info.st_ino, attributes.inode);
    ASSERT_EQ(info.st_nlink, attributes.nlink);
    ASSERT_EQ(info.st_mode, attributes.mode.to_mode());
    ASSERT_EQ(info.st_uid, attributes.uid.to_uid());
    ASSERT_EQ(info.st_gid, attributes.gid.to_gid());
    ASSERT_EQ(info.st_rdev, attributes.rdev);
    ASSERT_EQ(info.st_size, attributes.size);
    ASSERT_EQ(info.st_blocks, attributes.blocks);
    ASSERT_EQ(info.st_atim.tv_sec, attributes.atime.seconds);
    ASSERT_EQ(info.st_atim.tv_nsec, attributes.atime.nsec);
    ASSERT_EQ(info.st_mtim.tv_sec, attributes.mtime.seconds);
    ASSERT_EQ(info.st_mtim.tv_nsec, attributes.mtime.nsec);
    ASSERT_EQ(info.st_ctim.tv_sec, attributes.ctime.seconds);
    ASSERT_EQ(info.st_ctim.tv_nsec, attributes.ctime.nsec);
}

TEST(file_attibutes, to_stat)
{
    file_attributes attributes;
    attributes.inode = 1;
    attributes.nlink = 2;
    attributes.mode = filemode(S_IFREG | 0644);
    attributes.uid = user_id(1000);
    attributes.gid = group_id(1234);
    attributes.rdev = 0;
    attributes.size = 21 * 1024;
    attributes.blocks = 42;
    attributes.atime.seconds = 1;
    attributes.atime.nsec = 2;
    attributes.mtime.seconds = 3;
    attributes.mtime.nsec = 4;
    attributes.ctime.seconds = 5;
    attributes.ctime.nsec = 6;

    struct stat info;
    attributes.to_stat(info);

    ASSERT_EQ(attributes.inode, info.st_ino);
    ASSERT_EQ(attributes.nlink, info.st_nlink);
    ASSERT_EQ(attributes.mode.to_mode(), info.st_mode);
    ASSERT_EQ(attributes.uid.to_uid(), info.st_uid);
    ASSERT_EQ(attributes.gid.to_gid(), info.st_gid);
    ASSERT_EQ(attributes.rdev, info.st_rdev);
    ASSERT_EQ(attributes.size, info.st_size);
    ASSERT_EQ(attributes.blocks, info.st_blocks);
    ASSERT_EQ(attributes.atime.seconds, info.st_atim.tv_sec);
    ASSERT_EQ(attributes.atime.nsec, info.st_atim.tv_nsec);
    ASSERT_EQ(attributes.mtime.seconds, info.st_mtim.tv_sec);
    ASSERT_EQ(attributes.mtime.nsec, info.st_mtim.tv_nsec);
    ASSERT_EQ(attributes.ctime.seconds, info.st_ctim.tv_sec);
    ASSERT_EQ(attributes.ctime.nsec, info.st_ctim.tv_nsec);
}