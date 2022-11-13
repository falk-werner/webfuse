#include "webfuse/filesystem/filemode.hpp"
#include <sys/stat.h>

namespace webfuse
{

filemode::filemode(uint32_t value)
: value_(value)
{

}

filemode::operator uint32_t() const
{
    return value_;
}


filemode filemode::from_mode(mode_t value)
{
    uint32_t result = value & 07777;

    if (S_ISREG(value) ) { result |= filemode::reg;  }
    if (S_ISDIR(value) ) { result |= filemode::dir;  }
    if (S_ISCHR(value) ) { result |= filemode::chr;  }
    if (S_ISBLK(value) ) { result |= filemode::blk;  }
    if (S_ISFIFO(value)) { result |= filemode::fifo; }
    if (S_ISLNK(value) ) { result |= filemode::lnk;  }
    if (S_ISSOCK(value)) { result |= filemode::sock; }

    return filemode(result); 
}

mode_t filemode::to_mode() const
{
    mode_t result = value_ & 07777;

    if (is_reg() ) { result |= S_IFREG; }
    if (is_dir() ) { result |= S_IFDIR; }
    if (is_chr() ) { result |= S_IFCHR; }
    if (is_blk() ) { result |= S_IFBLK; }
    if (is_fifo()) { result |= S_IFIFO; }
    if (is_lnk() ) { result |= S_IFLNK; }
    if (is_sock()) { result |= S_IFSOCK; }

    return result;
}


}