#ifndef WEBFUSE_EMPTYFILESYSTEM_HPP
#define WEBFUSE_EMPTYFILESYSTEM_HPP

#include "webfuse/filesystem/filesystem_i.hpp"

namespace webfuse
{

class empty_filesystem: public filesystem_i
{
public:
    ~empty_filesystem() override = default;

    status access(std::string const & path, access_mode mode) override;
    status getattr(std::string const & path, file_attributes & attr) override;

    status readlink(std::string const & path, std::string & out) override;
    status symlink(std::string const & target, std::string const & linkpath) override;
    status link(std::string const & old_path, std::string const & new_path) override;

    status rename(std::string const & old_path, std::string const & new_path) override;
    status chmod(std::string const & path, filemode mode) override;
    status chown(std::string const & path, user_id uid, group_id gid) override;
    status truncate(std::string const & path, uint64_t offset, filehandle handle) override;
    status fsync(std::string const & path, bool is_datasync, filehandle handle) override;

    status open(std::string const & path, openflags flags, filehandle & handle) override;
    status mknod(std::string const & path, filemode mode, uint64_t rdev) override;
    status create(std::string const & path, filemode mode, filehandle & handle) override;
    status release(std::string const & path, filehandle handle) override;
    status unlink(std::string const & path) override;

    status read(std::string const & path, char * buffer, size_t buffer_size, uint64_t offset, filehandle handle) override;
    status write(std::string const & path, char const * buffer, size_t buffer_size, uint64_t offset, filehandle handle) override;

    status mkdir(std::string const & path, filemode mode) override;
    status readdir(std::string const & path, std::vector<std::string> & entries, filehandle handle) override;
    status rmdir(std::string const & path) override;

    status statfs(std::string const & path, filesystem_statistics & statistics) override;
};

}

#endif
