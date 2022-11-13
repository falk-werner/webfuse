#ifndef WEBFUSE_FILESYSTEM_I_HPP
#define WEBFUSE_FILESYSTEM_I_HPP

#include "webfuse/filesystem/filehandle.hpp"
#include "webfuse/filesystem/accessmode.hpp"
#include "webfuse/filesystem/filemode.hpp"
#include "webfuse/filesystem/fileattributes.hpp"
#include "webfuse/filesystem/openflags.hpp"
#include "webfuse/filesystem/userid.hpp"
#include "webfuse/filesystem/groupid.hpp"
#include "webfuse/filesystem/status.hpp"
#include "webfuse/filesystem/filesystem_statistics.hpp"

#include <cinttypes>
#include <string>
#include <vector>

namespace webfuse
{

class filesystem_i
{
public:
    virtual ~filesystem_i() = default;

    virtual status access(std::string const & path, access_mode mode) = 0;
    virtual status getattr(std::string const & path, file_attributes & attr) = 0;

    virtual status readlink(std::string const & path, std::string & out) = 0;
    virtual status symlink(std::string const & target, std::string const & linkpath) = 0;
    virtual status link(std::string const & old_path, std::string const & new_path) = 0;

    virtual status rename(std::string const & old_path, std::string const & new_path) = 0;
    virtual status chmod(std::string const & path, filemode mode) = 0;
    virtual status chown(std::string const & path, user_id uid, group_id gid) = 0;
    virtual status truncate(std::string const & path, uint64_t offset, filehandle handle) = 0;
    virtual status fsync(std::string const & path, bool is_datasync, filehandle handle) = 0;

    virtual status open(std::string const & path, openflags flags, filehandle & handle) = 0;
    virtual status mknod(std::string const & path, filemode mode, uint64_t rdev) = 0;
    virtual status create(std::string const & path, filemode mode, filehandle & handle) = 0;
    virtual status release(std::string const & path, filehandle handle) = 0;
    virtual status unlink(std::string const & path) = 0;

    virtual status read(std::string const & path, char * buffer, size_t buffer_size, uint64_t offset, filehandle handle) = 0;
    virtual status write(std::string const & path, char const * buffer, size_t buffer_size, uint64_t offset, filehandle handle) = 0;

    virtual status mkdir(std::string const & path, filemode mode) = 0;
    virtual status readdir(std::string const & path, std::vector<std::string> & entries, filehandle handle) = 0;
    virtual status rmdir(std::string const & path) = 0;

    virtual status statfs(std::string const & path, filesystem_statistics & statistics) = 0;
};

}

#endif
