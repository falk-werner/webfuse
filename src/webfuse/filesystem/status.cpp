#include "webfuse/filesystem/status.hpp"
#include <errno.h>

namespace webfuse
{

status::status(int32_t value)
: value_(value)
{

}

status::operator int32_t() const
{
    return value_;
}

status status::from_fusestatus(int value)
{
    if (value >= 0)
    {
        return static_cast<int32_t>(value);
    }
    else
    {
        switch(value)
        {
            case -E2BIG: return status::bad_e2big;
            case -EACCES:  return status::bad_eacces;
            case -EAGAIN: return status::bad_eagain;
            case -EBADF: return status::bad_ebadf;
            case -EBUSY: return status::bad_ebusy;
            case -EDESTADDRREQ: return status::bad_edestaddrreq;
            case -EDQUOT: return status::bad_edquot;
            case -EEXIST: return status::bad_eexist;
            case -EFAULT: return status::bad_efault;
            case -EFBIG: return status::bad_efbig;
            case -EINTR: return status::bad_eintr;
            case -EINVAL: return status::bad_einval;
            case -EIO: return status::bad_eio;
            case -EISDIR: return status::bad_eisdir;
            case -ELOOP: return status::bad_eloop;
            case -EMFILE: return status::bad_emfile;
            case -EMLINK: return status::bad_emlink;
            case -ENAMETOOLONG: return status::bad_enametoolong;
            case -ENFILE: return status::bad_enfile;
            case -ENODATA: return status::bad_enodata;
            case -ENODEV: return status::bad_enodev;
            case -ENOENT: return status::bad_enoent;
            case -ENOMEM: return status::bad_enomem;
            case -ENOSPC: return status::bad_enospc;
            case -ENOSYS: return status::bad_enosys;
            case -ENOTDIR: return status::bad_enotdir;
            case -ENOTEMPTY: return status::bad_enotempty;
            case -ENOTSUP: return status::bad_enotsup;
            case -ENXIO: return status::bad_enxio;
            case -EOVERFLOW: return status::bad_eoverflow;
            case -EPERM: return status ::bad_eperm;
            case -EPIPE: return status::bad_epipe;
            case -ERANGE: return status::bad_erange;
            case -EROFS: return status::bad_erofs;
            case -ETXTBSY: return status::bad_etxtbsy;
            case -EXDEV: return status::bad_exdev;
            default: return static_cast<int32_t>(value);
        }
    }
}

int status::to_fusestatus() const
{
    if (value_ >= 0)
    {
        return static_cast<int>(value_);
    }
    else
    {
        switch(value_)
        {
            case status::bad_e2big: return -E2BIG;
            case status::bad_eacces:  return -EACCES;
            case status::bad_eagain: return -EAGAIN;
            case status::bad_ebadf: return -EBADF;
            case status::bad_ebusy: return -EBUSY;
            case status::bad_edestaddrreq: return -EDESTADDRREQ;
            case status::bad_edquot: return -EDQUOT;
            case status::bad_eexist: return -EEXIST;
            case status::bad_efault: return -EFAULT;
            case status::bad_efbig: return -EFBIG;
            case status::bad_eintr: return -EINTR;
            case status::bad_einval: return -EINVAL;
            case status::bad_eio: return -EIO;
            case status::bad_eisdir: return -EISDIR;
            case status::bad_eloop: return -ELOOP;
            case status::bad_emfile: return -EMFILE;
            case status::bad_emlink: return -EMLINK;
            case status::bad_enametoolong: return -ENAMETOOLONG;
            case status::bad_enfile: return -ENFILE;
            case status::bad_enodata: return -ENODATA;
            case status::bad_enodev: return -ENODEV;
            case status::bad_enoent: return -ENOENT;
            case status::bad_enomem: return -ENOMEM;
            case status::bad_enospc: return -ENOSPC;
            case status::bad_enosys: return -ENOSYS;
            case status::bad_enotdir: return -ENOTDIR;
            case status::bad_enotempty: return -ENOTEMPTY;
            case status::bad_enotsup: return -ENOTSUP;
            case status::bad_enxio: return -ENXIO;
            case status::bad_eoverflow: return -EOVERFLOW;
            case status::bad_eperm: return -EPERM;
            case status::bad_epipe: return -EPIPE;
            case status::bad_erange: return -ERANGE;
            case status::bad_erofs: return -EROFS;
            case status::bad_etxtbsy: return -ETXTBSY;
            case status::bad_exdev: return -EXDEV;
            default: return static_cast<int32_t>(value_);
        }
    }
}

}