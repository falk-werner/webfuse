#include "webfuse/filesystem.hpp"
#include <errno.h>

namespace webfuse
{

filesystem::filesystem(ws_server& server)
: proxy(server)
{

}

filesystem::~filesystem()
{

}

int filesystem::access(std::string const & path, int mode)
{
    try
    {
        messagewriter req(message_type::access_req);
        req.write_str(path);
        req.write_access_mode(mode);
        auto reader = proxy.perform(std::move(req));
        return reader.read_result();
    }
    catch(...)
    {
        return fallback.access(path, mode);
    }
}

int filesystem::getattr(std::string const & path, struct stat * attr)
{
    try
    {
        messagewriter req(message_type::getattr_req);
        req.write_str(path);
        auto reader = proxy.perform(std::move(req));
        int const result = reader.read_result();
        if (0 == result)
        {
            reader.read_attr(attr);
        }
        return result;
    }
    catch(...)
    {
        return fallback.getattr(path, attr);
    }
}

int filesystem::readlink(std::string const & path, std::string & out)
{
    try
    {
        messagewriter req(message_type::readlink_req);
        proxy.perform(std::move(req));
        return -ENOENT;
    }
    catch(...)
    {
        return fallback.readlink(path, out);
    }
}

int filesystem::symlink(std::string const & target, std::string const & linkpath)
{
    try
    {
        messagewriter req(message_type::symlink_req);
        proxy.perform(std::move(req));
        return -ENOENT;
    }
    catch(...)
    {
        return fallback.symlink(target, linkpath);
    }
}

int filesystem::link(std::string const & old_path, std::string const & new_path)
{
    try
    {
        messagewriter req(message_type::link_req);
        proxy.perform(std::move(req));
        return -ENOENT;
    }
    catch(...)
    {
        return fallback.link(old_path, new_path);
    }
}

int filesystem::rename(std::string const & old_path, std::string const & new_path, int flags)
{
    try
    {
        messagewriter req(message_type::rename_req);
        proxy.perform(std::move(req));
        return -ENOENT;
    }
    catch(...)
    {
        return fallback.rename(old_path, new_path, flags);
    }
}

int filesystem::chmod(std::string const & path, mode_t mode)
{
    try
    {
        messagewriter req(message_type::chmod_req);
        proxy.perform(std::move(req));
        return -ENOENT;
    }
    catch(...)
    {
        return fallback.chmod(path, mode);
    }
}

int filesystem::chown(std::string const & path, uid_t uid, gid_t gid)
{
    try
    {
        messagewriter req(message_type::chown_req);
        proxy.perform(std::move(req));
        return -ENOENT;
    }
    catch(...)
    {
        return fallback.chown(path, uid, gid);
    }
}

int filesystem::truncate(std::string const & path, uint64_t size, uint64_t handle)
{
    try
    {
        messagewriter req(message_type::truncate_req);
        proxy.perform(std::move(req));
        return -ENOENT;
    }
    catch(...)
    {
        return fallback.truncate(path, size, handle);
    }
}

int filesystem::fsync(std::string const & path, bool is_datasync, uint64_t handle)
{
    try
    {
        messagewriter req(message_type::fsync_req);
        proxy.perform(std::move(req));
        return -ENOENT;
    }
    catch(...)
    {
        return fallback.fsync(path, is_datasync, handle);
    }
}

int filesystem::open(std::string const & path, int flags, uint64_t & handle)
{
    try
    {
        messagewriter req(message_type::open_req);
        proxy.perform(std::move(req));
        return -ENOENT;
    }
    catch(...)
    {
        return fallback.open(path, flags, handle);
    }
}

int filesystem::mknod(std::string const & path, mode_t mode, dev_t rdev)
{
    try
    {
        messagewriter req(message_type::mknod_req);
        proxy.perform(std::move(req));
        return -ENOENT;
    }
    catch(...)
    {
        return fallback.mknod(path, mode, rdev);
    }
}

int filesystem::create(std::string const & path, mode_t mode, uint64_t & handle)
{
    try
    {
        messagewriter req(message_type::create_req);
        proxy.perform(std::move(req));
        return -ENOENT;
    }
    catch(...)
    {
        return fallback.create(path, mode, handle);
    }
}

int filesystem::release(std::string const & path, uint64_t handle)
{
    try
    {
        messagewriter req(message_type::release_req);
        proxy.perform(std::move(req));
        return -ENOENT;
    }
    catch(...)
    {
        return fallback.release(path, handle);
    }
}

int filesystem::unlink(std::string const & path)
{
    try
    {
        messagewriter req(message_type::unlink_req);
        proxy.perform(std::move(req));
        return -ENOENT;
    }
    catch(...)
    {
        return fallback.unlink(path);
    }
}

int filesystem::read(std::string const & path, char * buffer, size_t buffer_size, uint64_t offset, uint64_t handle)
{
    try
    {
        messagewriter req(message_type::read_req);
        proxy.perform(std::move(req));
        return -ENOENT;
    }
    catch(...)
    {
        return fallback.read(path, buffer, buffer_size, offset, handle);
    }
}

int filesystem::write(std::string const & path, char const * buffer, size_t buffer_size, uint64_t offset, uint64_t handle)
{
    try
    {
        messagewriter req(message_type::write_req);
        proxy.perform(std::move(req));
        return -ENOENT;
    }
    catch(...)
    {
        return fallback.write(path, buffer, buffer_size, offset, handle);
    }
}

int filesystem::mkdir(std::string const & path, mode_t mode)
{
    try
    {
        messagewriter req(message_type::mkdir_req);
        proxy.perform(std::move(req));
        return -ENOENT;
    }
    catch(...)
    {
        return fallback.mkdir(path, mode);
    }
}

int filesystem::readdir(std::string const & path, std::vector<std::string> & entries, uint64_t handle)
{
    try
    {
        messagewriter req(message_type::readdir_req);
        req.write_str(path);        
        auto resp = proxy.perform(std::move(req));
        int result = resp.read_result();
        if (0 == result)
        {
            resp.read_strings(entries);
        }
        return result;
    }
    catch(...)
    {
        return fallback.readdir(path, entries, handle);
    }
}

int filesystem::rmdir(std::string const & path)
{
    try
    {
        messagewriter req(message_type::rmdir_req);
        proxy.perform(std::move(req));
        return -ENOENT;
    }
    catch(...)
    {
        return fallback.rmdir(path);
    }
}

int filesystem::statfs(std::string const & path, struct statvfs * statistics)
{
    try
    {
        messagewriter req(message_type::statfs_req);
        proxy.perform(std::move(req));
        return -ENOENT;
    }
    catch(...)
    {
        return fallback.statfs(path, statistics);
    }
}

}