#include "webfuse/filesystem/empty_filesystem.hpp"
#include <errno.h>

namespace webfuse
{

int empty_filesystem::access(std::string const & path, int mode)
{
    if (path == "/")
    {
        return 0;
    }
    else
    {
        return -ENOENT;
    }
}

int empty_filesystem::getattr(std::string const & path, struct stat * attr)
{
    if (path == "/")
    {
        attr->st_ino = 1;
        attr->st_nlink = 1;
        attr->st_mode = S_IFDIR | 0555;
        return 0;
    }
    else
    {
        return -ENOENT;
    }
}

int empty_filesystem::readlink(std::string const & path, std::string & out)
{
    return -ENOENT;
}

int empty_filesystem::symlink(std::string const & target, std::string const & linkpath)
{
    return -ENOENT;
}

int empty_filesystem::link(std::string const & old_path, std::string const & new_path)
{
    return -ENOENT;
}

int empty_filesystem::rename(std::string const & old_path, std::string const & new_path, int flags)
{
    return -ENOENT;
}

int empty_filesystem::chmod(std::string const & path, mode_t mode)
{
    return -EPERM;
}

int empty_filesystem::chown(std::string const & path, uid_t uid, gid_t gid)
{
    return -EPERM;
}

int empty_filesystem::truncate(std::string const & path, uint64_t size, uint64_t handle)
{
    return -EPERM;
}

int empty_filesystem::fsync(std::string const & path, bool is_datasync, uint64_t handle)
{
    return 0;
}

int empty_filesystem::utimens(std::string const &path, struct timespec tv[2], uint64_t handle)
{
    return -ENOSYS;
}

int empty_filesystem::open(std::string const & path, int flags, uint64_t & handle)
{
    return -ENOENT;
}

int empty_filesystem::mknod(std::string const & path, mode_t mode, dev_t rdev)
{
    return -EPERM;

}
int empty_filesystem::create(std::string const & path, mode_t mode, uint64_t & handle)
{
    return -EPERM;
}

int empty_filesystem::release(std::string const & path, uint64_t handle)
{
    return 0;
}

int empty_filesystem::unlink(std::string const & path)
{
    return -EPERM;
}

int empty_filesystem::read(std::string const & path, char * buffer, size_t buffer_size, uint64_t offset, uint64_t handle)
{
    return -EBADF;
}
int empty_filesystem::write(std::string const & path, char const * buffer, size_t buffer_size, uint64_t offset, uint64_t handle)
{
    return -EBADF;
}

int empty_filesystem::mkdir(std::string const & path, mode_t mode)
{
    return -EPERM;
}

int empty_filesystem::readdir(std::string const & path, std::vector<std::string> & entries)
{
    if (path == "/")
    {
        return 0;
    }
    else
    {
        return -ENOENT;
    }
}

int empty_filesystem::rmdir(std::string const & path)
{
    return -EPERM;
}

int empty_filesystem::statfs(std::string const & path, struct statvfs * statistics)
{
    return -ENOSYS;
}


}