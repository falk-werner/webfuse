#ifndef WEBFUSE_FILESYSTEMSTATISTICS_HPP
#define WEBFUSE_FILESYSTEMSTATISTICS_HPP

#include <sys/statvfs.h>
#include <cinttypes>

namespace webfuse
{

class filesystem_statistics
{
public:
    filesystem_statistics();
    explicit filesystem_statistics(struct statvfs const & other);
    void copy_to(struct statvfs & other) const;

    uint64_t bsize;
    uint64_t frsize;
    uint64_t blocks;
    uint64_t bfree;
    uint64_t bavail;
    uint64_t files;
    uint64_t ffree;
    uint64_t f_namemax;
};



}

#endif
