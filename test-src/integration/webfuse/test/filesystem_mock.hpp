#ifndef WEBFUSE_FILESYSTEM_MOCK_HPP
#define WEBFUSE_FILESYSTEM_MOCK_HPP

#include "webfuse/filesystem/filesystem_i.hpp"
#include <gmock/gmock.h>

namespace webfuse
{

class filesystem_mock: public filesystem_i
{
public:
    ~filesystem_mock() override = default;

    MOCK_METHOD(int, access, (std::string const & path, int mode));
    MOCK_METHOD(int, getattr, (std::string const & path, struct stat * attr));

    MOCK_METHOD(int, readlink, (std::string const & path, std::string & out));
    MOCK_METHOD(int, symlink, (std::string const & target, std::string const & linkpath));
    MOCK_METHOD(int, link, (std::string const & old_path, std::string const & new_path));

    MOCK_METHOD(int, rename, (std::string const & old_path, std::string const & new_path, int flags));
    MOCK_METHOD(int, chmod, (std::string const & path, mode_t mode));
    MOCK_METHOD(int, chown, (std::string const & path, uid_t uid, gid_t gid));
    MOCK_METHOD(int, truncate, (std::string const & path, uint64_t size, uint64_t handle));
    MOCK_METHOD(int, fsync, (std::string const & path, bool is_datasync, uint64_t handle));
    MOCK_METHOD(int, utimens, (std::string const &path, struct timespec tv[2], uint64_t handle)); 

    MOCK_METHOD(int, open, (std::string const & path, int flags, uint64_t & handle));
    MOCK_METHOD(int, mknod, (std::string const & path, mode_t mode, dev_t rdev));
    MOCK_METHOD(int, create, (std::string const & path, mode_t mode, uint64_t & handle));
    MOCK_METHOD(int, release, (std::string const & path, uint64_t handle));
    MOCK_METHOD(int, unlink, (std::string const & path));

    MOCK_METHOD(int, read, (std::string const & path, char * buffer, size_t buffer_size, uint64_t offset, uint64_t handle));
    MOCK_METHOD(int, write, (std::string const & path, char const * buffer, size_t buffer_size, uint64_t offset, uint64_t handle));

    MOCK_METHOD(int, mkdir, (std::string const & path, mode_t mode));
    MOCK_METHOD(int, readdir, (std::string const & path, std::vector<std::string> & entries));
    MOCK_METHOD(int, rmdir, (std::string const & path));

    MOCK_METHOD(int, statfs, (std::string const & path, struct statvfs * statistics));
};

}

#endif
