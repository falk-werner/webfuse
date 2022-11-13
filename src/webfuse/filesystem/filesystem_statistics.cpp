#include "webfuse/filesystem/filesystem_statistics.hpp"

namespace webfuse
{

filesystem_statistics::filesystem_statistics()
: bsize(0)
, frsize(0)
, blocks(0)
, bfree(0)
, bavail(0)
, files(0)
, ffree(0)
, f_namemax(0)
{

}

filesystem_statistics::filesystem_statistics(struct statvfs const & other)
{
    bsize = other.f_bsize;
    frsize = other.f_frsize;
    blocks = other.f_blocks;
    bfree = other.f_bfree;
    bavail = other.f_bavail;
    files = other.f_files;
    ffree = other.f_ffree;
    f_namemax = other.f_namemax;
}

void filesystem_statistics::copy_to(struct statvfs & other) const
{
    other.f_bsize = bsize;
    other.f_frsize = frsize;
    other.f_blocks = blocks;
    other.f_bfree = bfree;
    other.f_bavail = bavail;
    other.f_files = files;
    other.f_ffree = ffree;
    other.f_namemax = f_namemax;
}

}