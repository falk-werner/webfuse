#ifndef WEBFUSE_FILEATTRIBUTES_HPP
#define WEBFUSE_FILEATTRIBUTES_HPP

#include "webfuse/filesystem/filemode.hpp"
#include "webfuse/filesystem/filetime.hpp"
#include "webfuse/filesystem/userid.hpp"
#include "webfuse/filesystem/groupid.hpp"
#include <sys/stat.h>
#include <cinttypes>

namespace webfuse
{

class file_attributes
{
public:
    file_attributes();

    explicit file_attributes(struct stat const & other);
    void to_stat(struct stat & other) const;

    uint64_t inode;
    uint64_t nlink;
    filemode mode;
    user_id uid;
    group_id gid;
    uint64_t rdev;
    uint64_t size;
    uint64_t blocks;
    filetime atime;
    filetime mtime;
    filetime ctime;
};

}

#endif
