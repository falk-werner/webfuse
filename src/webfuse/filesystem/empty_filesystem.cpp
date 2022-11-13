#include "webfuse/filesystem/empty_filesystem.hpp"

namespace webfuse
{

status empty_filesystem::access(std::string const & path, access_mode mode)
{
    if (path == "/")
    {
        return status::good;
    }
    else
    {
        return status::bad_enoent;
    }
}

status empty_filesystem::getattr(std::string const & path, file_attributes & attr)
{
    if (path == "/")
    {
        attr.inode = 1;
        attr.nlink = 1;
        attr.mode = filemode(filemode::dir | 0x444);
        return status::good;
    }
    else
    {
        return status::bad_enoent;
    }
}

status empty_filesystem::readlink(std::string const & path, std::string & out)
{
    return status::bad_enoent;
}

status empty_filesystem::symlink(std::string const & target, std::string const & linkpath)
{
    return status::bad_enoent;
}

status empty_filesystem::link(std::string const & old_path, std::string const & new_path)
{
    return status::bad_enoent;
}

status empty_filesystem::rename(std::string const & old_path, std::string const & new_path)
{
    return status::bad_enoent;
}

status empty_filesystem::chmod(std::string const & path, filemode mode)
{
    return status::bad_eperm;
}

status empty_filesystem::chown(std::string const & path, user_id uid, group_id gid)
{
    return status::bad_eperm;
}

status empty_filesystem::truncate(std::string const & path, uint64_t offset, filehandle handle)
{
    return status::bad_eperm;
}

status empty_filesystem::fsync(std::string const & path, bool is_datasync, filehandle handle)
{
    return status::good;
}

status empty_filesystem::open(std::string const & path, openflags flags, filehandle & handle)
{
    return status::bad_enoent;
}

status empty_filesystem::mknod(std::string const & path, filemode mode, uint64_t rdev)
{
    return status::bad_eperm;

}
status empty_filesystem::create(std::string const & path, filemode mode, filehandle & handle)
{
    return status::bad_eperm;
}

status empty_filesystem::release(std::string const & path, filehandle handle)
{
    return status::good;
}

status empty_filesystem::unlink(std::string const & path)
{
    return status::bad_eperm;
}

status empty_filesystem::read(std::string const & path, char * buffer, size_t buffer_size, uint64_t offset, filehandle handle)
{
    return status::bad_ebadf;
}
status empty_filesystem::write(std::string const & path, char const * buffer, size_t buffer_size, uint64_t offset, filehandle handle)
{
    return status::bad_ebadf;
}

status empty_filesystem::mkdir(std::string const & path, filemode mode)
{
    return status::bad_eperm;
}

status empty_filesystem::readdir(std::string const & path, std::vector<std::string> & entries, filehandle handle)
{
    if (path == "/")
    {
        return status::good;
    }
    else
    {
        return status::bad_enoent;
    }
}

status empty_filesystem::rmdir(std::string const & path)
{
    return status::bad_eperm;
}

status empty_filesystem::statfs(std::string const & path, filesystem_statistics & statistics)
{
    return status::bad_enosys;
}


}