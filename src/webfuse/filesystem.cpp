#include "webfuse/filesystem.hpp"

namespace webfuse
{

filesystem::filesystem(ws_server& server)
: proxy(server)
{

}

filesystem::~filesystem()
{

}

status filesystem::access(std::string const & path, access_mode mode)
{
    try
    {
        std::string req;
        auto resp = proxy.perform(req).get();
        return status::bad_enoent;
    }
    catch(...)
    {
        return fallback.access(path, mode);
    }
}

status filesystem::getattr(std::string const & path, file_attributes & attr)
{
    try
    {
        std::string req;
        auto resp = proxy.perform(req).get();
        return status::bad_enoent;
    }
    catch(...)
    {
        return fallback.getattr(path, attr);
    }
}

status filesystem::readlink(std::string const & path, std::string & out)
{
    try
    {
        std::string req;
        auto resp = proxy.perform(req).get();
        return status::bad_enoent;
    }
    catch(...)
    {
        return fallback.readlink(path, out);
    }
}

status filesystem::symlink(std::string const & target, std::string const & linkpath)
{
    try
    {
        std::string req;
        auto resp = proxy.perform(req).get();
        return status::bad_enoent;
    }
    catch(...)
    {
        return fallback.symlink(target, linkpath);
    }
}

status filesystem::link(std::string const & old_path, std::string const & new_path)
{
    try
    {
        std::string req;
        auto resp = proxy.perform(req).get();
        return status::bad_enoent;
    }
    catch(...)
    {
        return fallback.link(old_path, new_path);
    }
}

status filesystem::rename(std::string const & old_path, std::string const & new_path)
{
    try
    {
        std::string req;
        auto resp = proxy.perform(req).get();
        return status::bad_enoent;
    }
    catch(...)
    {
        return fallback.rename(old_path, new_path);
    }
}

status filesystem::chmod(std::string const & path, filemode mode)
{
    try
    {
        std::string req;
        auto resp = proxy.perform(req).get();
        return status::bad_enoent;
    }
    catch(...)
    {
        return fallback.chmod(path, mode);
    }
}

status filesystem::chown(std::string const & path, user_id uid, group_id gid)
{
    try
    {
        std::string req;
        auto resp = proxy.perform(req).get();
        return status::bad_enoent;
    }
    catch(...)
    {
        return fallback.chown(path, uid, gid);
    }
}

status filesystem::truncate(std::string const & path, uint64_t offset, filehandle handle)
{
    try
    {
        std::string req;
        auto resp = proxy.perform(req).get();
        return status::bad_enoent;
    }
    catch(...)
    {
        return fallback.truncate(path, offset, handle);
    }
}

status filesystem::fsync(std::string const & path, bool is_datasync, filehandle handle)
{
    try
    {
        std::string req;
        auto resp = proxy.perform(req).get();
        return status::bad_enoent;
    }
    catch(...)
    {
        return fallback.fsync(path, is_datasync, handle);
    }
}

status filesystem::open(std::string const & path, openflags flags, filehandle & handle)
{
    try
    {
        std::string req;
        auto resp = proxy.perform(req).get();
        return status::bad_enoent;
    }
    catch(...)
    {
        return fallback.open(path, flags, handle);
    }
}

status filesystem::mknod(std::string const & path, filemode mode, uint64_t rdev)
{
    try
    {
        std::string req;
        auto resp = proxy.perform(req).get();
        return status::bad_enoent;
    }
    catch(...)
    {
        return fallback.mknod(path, mode, rdev);
    }
}

status filesystem::create(std::string const & path, filemode mode, filehandle & handle)
{
    try
    {
        std::string req;
        auto resp = proxy.perform(req).get();
        return status::bad_enoent;
    }
    catch(...)
    {
        return fallback.create(path, mode, handle);
    }
}

status filesystem::release(std::string const & path, filehandle handle)
{
    try
    {
        std::string req;
        auto resp = proxy.perform(req).get();
        return status::bad_enoent;
    }
    catch(...)
    {
        return fallback.release(path, handle);
    }
}

status filesystem::unlink(std::string const & path)
{
    try
    {
        std::string req;
        auto resp = proxy.perform(req).get();
        return status::bad_enoent;
    }
    catch(...)
    {
        return fallback.unlink(path);
    }
}

status filesystem::read(std::string const & path, char * buffer, size_t buffer_size, uint64_t offset, filehandle handle)
{
    try
    {
        std::string req;
        auto resp = proxy.perform(req).get();
        return status::bad_enoent;
    }
    catch(...)
    {
        return fallback.read(path, buffer, buffer_size, offset, handle);
    }
}

status filesystem::write(std::string const & path, char const * buffer, size_t buffer_size, uint64_t offset, filehandle handle)
{
    try
    {
        std::string req;
        auto resp = proxy.perform(req).get();
        return status::bad_enoent;
    }
    catch(...)
    {
        return fallback.write(path, buffer, buffer_size, offset, handle);
    }
}

status filesystem::mkdir(std::string const & path, filemode mode)
{
    try
    {
        std::string req;
        auto resp = proxy.perform(req).get();
        return status::bad_enoent;
    }
    catch(...)
    {
        return fallback.mkdir(path, mode);
    }
}

status filesystem::readdir(std::string const & path, std::vector<std::string> & entries, filehandle handle)
{
    try
    {
        std::string req;
        auto resp = proxy.perform(req).get();
        return status::bad_enoent;
    }
    catch(...)
    {
        return fallback.readdir(path, entries, handle);
    }
}

status filesystem::rmdir(std::string const & path)
{
    try
    {
        std::string req;
        auto resp = proxy.perform(req).get();
        return status::bad_enoent;
    }
    catch(...)
    {
        return fallback.rmdir(path);
    }
}

status filesystem::statfs(std::string const & path, filesystem_statistics & statistics)
{
    try
    {
        std::string req;
        auto resp = proxy.perform(req).get();
        return status::bad_enoent;
    }
    catch(...)
    {
        return fallback.statfs(path, statistics);
    }
}

}