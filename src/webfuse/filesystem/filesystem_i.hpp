#ifndef WEBFUSE_FILESYSTEM_I_HPP
#define WEBFUSE_FILESYSTEM_I_HPP

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <unistd.h>

#include <cinttypes>
#include <string>
#include <vector>

namespace webfuse
{

class filesystem_i
{
public:
    virtual ~filesystem_i() = default;

    virtual int access(std::string const & path, int mode) = 0;
    virtual int getattr(std::string const & path, struct stat * attr) = 0;

    virtual int readlink(std::string const & path, std::string & out) = 0;
    virtual int symlink(std::string const & target, std::string const & linkpath) = 0;
    virtual int link(std::string const & old_path, std::string const & new_path) = 0;

    virtual int rename(std::string const & old_path, std::string const & new_path, int flags) = 0;
    virtual int chmod(std::string const & path, mode_t mode) = 0;
    virtual int chown(std::string const & path, uid_t uid, gid_t gid) = 0;
    virtual int truncate(std::string const & path, uint64_t size, uint64_t handle) = 0;
    virtual int fsync(std::string const & path, bool is_datasync, uint64_t handle) = 0;
    virtual int utimens(std::string const &path, struct timespec const tv[2], uint64_t handle) = 0; 

    virtual int open(std::string const & path, int flags, uint64_t & handle) = 0;
    virtual int mknod(std::string const & path, mode_t mode, dev_t rdev) = 0;
    virtual int create(std::string const & path, mode_t mode, uint64_t & handle) = 0;
    virtual int release(std::string const & path, uint64_t handle) = 0;
    virtual int unlink(std::string const & path) = 0;

    virtual int read(std::string const & path, char * buffer, size_t buffer_size, uint64_t offset, uint64_t handle) = 0;
    virtual int write(std::string const & path, char const * buffer, size_t buffer_size, uint64_t offset, uint64_t handle) = 0;

    virtual int mkdir(std::string const & path, mode_t mode) = 0;
    virtual int readdir(std::string const & path, std::vector<std::string> & entries) = 0;
    virtual int rmdir(std::string const & path) = 0;

    virtual int statfs(std::string const & path, struct statvfs * statistics) = 0;
};

}

#endif
