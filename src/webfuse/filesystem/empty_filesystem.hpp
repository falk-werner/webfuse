#ifndef WEBFUSE_EMPTYFILESYSTEM_HPP
#define WEBFUSE_EMPTYFILESYSTEM_HPP

#include "webfuse/filesystem/filesystem_i.hpp"

namespace webfuse
{

class empty_filesystem: public filesystem_i
{
public:
    ~empty_filesystem() override = default;

    int access(std::string const & path, int mode) override;
    int getattr(std::string const & path, struct stat * attr) override;

    int readlink(std::string const & path, std::string & out) override;
    int symlink(std::string const & target, std::string const & linkpath) override;
    int link(std::string const & old_path, std::string const & new_path) override;

    int rename(std::string const & old_path, std::string const & new_path, int flags) override;
    int chmod(std::string const & path, mode_t mode) override;
    int chown(std::string const & path, uid_t uid, gid_t gid) override;
    int truncate(std::string const & path, uint64_t size, uint64_t handle) override;
    int fsync(std::string const & path, bool is_datasync, uint64_t handle) override;

    int open(std::string const & path, int flags, uint64_t & handle) override;
    int mknod(std::string const & path, mode_t mode, dev_t rdev) override;
    int create(std::string const & path, mode_t mode, uint64_t & handle) override;
    int release(std::string const & path, uint64_t handle) override;
    int unlink(std::string const & path) override;

    int read(std::string const & path, char * buffer, size_t buffer_size, uint64_t offset, uint64_t handle) override;
    int write(std::string const & path, char const * buffer, size_t buffer_size, uint64_t offset, uint64_t handle) override;

    int mkdir(std::string const & path, mode_t mode) override;
    int readdir(std::string const & path, std::vector<std::string> & entries, uint64_t handle) override;
    int rmdir(std::string const & path) override;

    int statfs(std::string const & path, struct statvfs * statistivs) override;
};

}

#endif
