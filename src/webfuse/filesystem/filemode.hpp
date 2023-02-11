#ifndef WEBFUSE_FILEMODE_HPP
#define WEBFUSE_FILEMODE_HPP

#include <fcntl.h>
#include <cinttypes>

namespace webfuse
{

class filemode
{
public:
    static constexpr uint32_t const protection_mask = 0000777;
    static constexpr uint32_t const sticky_mask     = 0007000;
    static constexpr uint32_t const filetype_mask   = 0170000;

    static constexpr uint32_t const suid  = 04000; // S_ISUID
    static constexpr uint32_t const sgid  = 02000; // S_ISGID
    static constexpr uint32_t const svtx  = 01000; // S_ISVTX
 
    static constexpr uint32_t const reg  = 0100000; // S_IFREG
    static constexpr uint32_t const dir  = 0040000; // S_IFDIR
    static constexpr uint32_t const chr  = 0020000; // S_IFCHR
    static constexpr uint32_t const blk  = 0060000; // S_IFBLK
    static constexpr uint32_t const fifo = 0010000; // S_IFIFO
    static constexpr uint32_t const lnk  = 0120000; // S_IFLNK
    static constexpr uint32_t const sock = 0140000; // S_IFSOCK

    explicit filemode(uint32_t value = 0);
    operator uint32_t() const;
    static filemode from_mode(mode_t value);
    mode_t to_mode() const;

    inline bool is_reg() const 
    { 
        return (filemode::reg == (value_ & filemode::filetype_mask));
    }

    inline bool is_dir() const 
    { 
        return (filemode::dir == (value_ & filemode::filetype_mask));
    }

    inline bool is_chr() const 
    { 
        return (filemode::chr == (value_ & filemode::filetype_mask));
    }

    inline bool is_blk() const 
    { 
        return (filemode::blk == (value_ & filemode::filetype_mask));
    }

    inline bool is_fifo() const 
    { 
        return (filemode::fifo == (value_ & filemode::filetype_mask));
    }

    inline bool is_lnk() const 
    { 
        return (filemode::lnk == (value_ & filemode::filetype_mask));
    }

    inline bool is_sock() const 
    { 
        return (filemode::sock == (value_ & filemode::filetype_mask));
    }

private:
    uint32_t value_;
};

}

#endif
