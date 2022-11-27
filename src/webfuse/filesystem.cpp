#include "webfuse/filesystem.hpp"

#include <errno.h>
#include <cstring>
#include <stdexcept>

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
        req.write_str(path);
        auto reader = proxy.perform(std::move(req));
        int const result = reader.read_result();
        if (0 == result)
        {
            out = reader.read_str();
        }
        return result;
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
        req.write_str(target);
        req.write_str(linkpath);
        auto reader = proxy.perform(std::move(req));        
        return reader.read_result();
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
        req.write_str(old_path);
        req.write_str(new_path);
        auto reader = proxy.perform(std::move(req));
        return reader.read_result();
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
        req.write_str(old_path);
        req.write_str(new_path);
        req.write_rename_flags(flags);
        auto reader = proxy.perform(std::move(req));
        return reader.read_result();
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
        req.write_str(path);
        req.write_mode(mode);
        auto reader = proxy.perform(std::move(req));
        return reader.read_result();
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
        req.write_str(path);
        req.write_uid(uid);
        req.write_gid(gid);
        auto reader = proxy.perform(std::move(req));
        return reader.read_result();
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
        req.write_str(path);
        req.write_u64(size);
        req.write_u64(handle);
        auto reader = proxy.perform(std::move(req));
        return reader.read_result();
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
        req.write_str(path);
        req.write_bool(is_datasync);
        req.write_u64(handle);
        auto reader = proxy.perform(std::move(req));
        return reader.read_result();
    }
    catch(...)
    {
        return fallback.fsync(path, is_datasync, handle);
    }
}

int filesystem::utimens(std::string const &path, struct timespec tv[2], uint64_t handle)
{
    try
    {
        messagewriter req(message_type::utimens_req);
        req.write_str(path);
        req.write_time(tv[0]);
        req.write_time(tv[1]);
        req.write_u64(handle);
        auto reader = proxy.perform(std::move(req));
        return reader.read_result();
    }
    catch(...)
    {
        return fallback.utimens(path, tv, handle);
    }
}


int filesystem::open(std::string const & path, int flags, uint64_t & handle)
{
    try
    {
        messagewriter req(message_type::open_req);
        req.write_str(path);
        req.write_openflags(flags);
        auto reader = proxy.perform(std::move(req));
        int result = reader.read_result();
        if (0 == result)
        {
            handle = reader.read_u64();
        }
        return result;
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
        req.write_str(path);
        req.write_mode(mode);
        req.write_u64(rdev);
        auto reader = proxy.perform(std::move(req));
        return reader.read_result();
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
        req.write_str(path);
        req.write_mode(mode);
        auto reader = proxy.perform(std::move(req));
        int result = reader.read_result();
        if (0 == result)
        {
            handle = reader.read_u64();
        }
        return result;
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
        req.write_str(path);
        req.write_u64(handle);
        auto reader = proxy.perform(std::move(req));
        return reader.read_result();
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
        req.write_str(path);
        auto reader = proxy.perform(std::move(req));
        return reader.read_result();
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
        req.write_str(path);
        req.write_u32(buffer_size);
        req.write_u64(offset);
        req.write_u64(handle);
        auto reader = proxy.perform(std::move(req));
        int result = reader.read_result();
        if (result > 0)
        {
            std::string data = reader.read_bytes();
            if (data.size() <= buffer_size)
            {
                memcpy(reinterpret_cast<void*>(buffer), reinterpret_cast<void const*>(data.data()), data.size());
            }
            else
            {
                throw std::runtime_error("invalid message");
            }
        }
        return result;
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
        req.write_str(path);
        req.write_data(buffer, buffer_size);
        req.write_u64(offset);
        req.write_u64(handle);
        auto reader = proxy.perform(std::move(req));
        return reader.read_result();
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
        req.write_str(path);
        req.write_mode(mode);
        auto reader = proxy.perform(std::move(req));
        return reader.read_result();
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
        req.write_str(path);
        auto reader = proxy.perform(std::move(req));
        return reader.read_result();
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
        req.write_str(path);
        auto reader = proxy.perform(std::move(req));
        int result = reader.read_result();
        if (0 == result)
        {
            reader.read_statistics(statistics);
        }
        return result;
    }
    catch(...)
    {
        return fallback.statfs(path, statistics);
    }
}

}