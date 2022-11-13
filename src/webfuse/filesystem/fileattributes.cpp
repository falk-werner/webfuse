#include "webfuse/filesystem/fileattributes.hpp"

namespace webfuse
{

file_attributes::file_attributes()
: inode(0)
, nlink(0)
, rdev(0)
, size(0)
, blocks(0)
{

}

file_attributes::file_attributes(struct stat const & other)
{
    inode = static_cast<uint64_t>(other.st_ino);
    nlink = static_cast<uint64_t>(other.st_nlink);
    mode = filemode::from_mode(other.st_mode);
    uid = user_id::from_uid(other.st_uid);
    gid = group_id::from_gid(other.st_gid);
    rdev = static_cast<uint64_t>(other.st_rdev);
    size = static_cast<uint64_t>(other.st_size);
    blocks = static_cast<uint64_t>(other.st_blocks);
    atime = filetime::from_timespec(other.st_atim);
    mtime = filetime::from_timespec(other.st_mtim);
    ctime = filetime::from_timespec(other.st_ctim);
}

void file_attributes::to_stat(struct stat & other) const
{
    other.st_ino = inode;
    other.st_nlink = nlink;
    other.st_mode = mode.to_mode();
    other.st_uid = uid.to_uid();
    other.st_gid = gid.to_gid();
    other.st_rdev = rdev;
    other.st_size = size;
    other.st_blocks = blocks;
    atime.to_timespec(other.st_atim);
    mtime.to_timespec(other.st_mtim);
    ctime.to_timespec(other.st_ctim);
}

}