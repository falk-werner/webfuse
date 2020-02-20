#include "webfuse/utils/file_utils.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

namespace webfuse_test
{

bool is_dir(std::string const & path)
{
    struct stat info;
    int rc = stat(path.c_str(), &info);
    
    return (0 == rc) && (S_ISDIR(info.st_mode));
}

bool is_symlink(std::string const & path)
{
    struct stat info;
    int rc = lstat(path.c_str(), &info);
    
    return (0 == rc) && (S_ISLNK(info.st_mode));
}

bool is_same_path(std::string const & path, std::string const & other)
{
    struct stat info;
    int rc = stat(path.c_str(), &info);

    struct stat info_other;
    int rc_other = stat(other.c_str(), &info_other);

    return (0 == rc) && (0 == rc_other) && (info.st_ino == info_other.st_ino);
}


}