#ifndef WEBFUSE_STATUS_HPP
#define WEBFUSE_STATUS_HPP

#include <cinttypes>

namespace webfuse
{

class status
{
public:
    static constexpr int32_t const good             =    0;
    static constexpr int32_t const bad_e2big        =   -7; // -E2BIG
    static constexpr int32_t const bad_eacces       =  -13; // -EACCES
    static constexpr int32_t const bad_eagain       =  -11; // -EAGAIN
    static constexpr int32_t const bad_ebadf        =   -9; // -EBADF
    static constexpr int32_t const bad_ebusy        =  -16; // -EBUSY
    static constexpr int32_t const bad_edestaddrreq =  -89; // -EDESTADDRREQ
    static constexpr int32_t const bad_edquot       = -122; // -EDQUOT
    static constexpr int32_t const bad_eexist       =  -17; // -EEXIST
    static constexpr int32_t const bad_efault       =  -14; // -EFAULT
    static constexpr int32_t const bad_efbig        =  -27; // -EFBIG
    static constexpr int32_t const bad_eintr        =   -4; // -EINTR
    static constexpr int32_t const bad_einval       =  -22; // -EINVAL
    static constexpr int32_t const bad_eio          =   -5; // -EIO
    static constexpr int32_t const bad_eisdir       =  -21; // -EISDIR
    static constexpr int32_t const bad_eloop        =  -40; // -ELOOP
    static constexpr int32_t const bad_emfile       =  -24; // -EMFILE
    static constexpr int32_t const bad_emlink       =  -31; // -EMLINK
    static constexpr int32_t const bad_enametoolong =  -36; // -ENAMETOOLONG
    static constexpr int32_t const bad_enfile       =  -23; // -ENFILE
    static constexpr int32_t const bad_enodata      =  -61; // -ENODATA
    static constexpr int32_t const bad_enodev       =  -19; // -ENODEV
    static constexpr int32_t const bad_enoent       =   -2; // -ENOENT
    static constexpr int32_t const bad_enomem       =  -12; // -ENOMEM
    static constexpr int32_t const bad_enospc       =  -28; // -ENOSPC
    static constexpr int32_t const bad_enosys       =  -38; // -ENOSYS
    static constexpr int32_t const bad_enotdir      =  -20; // -ENOTDIR
    static constexpr int32_t const bad_enotempty    =  -39; // -ENOTEMPTY
    static constexpr int32_t const bad_enotsup      =  -95; // -ENOTSUP
    static constexpr int32_t const bad_enxio        =   -6; // -ENXIO
    static constexpr int32_t const bad_eoverflow    =  -75; // -EOVERFLOW
    static constexpr int32_t const bad_eperm        =   -1; // -EPERM
    static constexpr int32_t const bad_epipe        =  -32; // -EPIPE
    static constexpr int32_t const bad_erange       =  -34; // -ERANGE
    static constexpr int32_t const bad_erofs        =  -30; // -EROFS
    static constexpr int32_t const bad_etxtbsy      =  -26; // -ETXTBSY
    static constexpr int32_t const bad_exdev        =  -18; // -EXDEV
    static constexpr int32_t const bad_ewouldblock  =  -11; // -EWOULDBLOCK

    status(int32_t value = status::good);
    operator int32_t() const;

    static status from_fusestatus(int value);
    int to_fusestatus() const;

    bool is_good() const;
private:
    int32_t value_;
};

}

#endif
