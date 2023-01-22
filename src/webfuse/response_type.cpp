#include "webfuse/response_type.hpp"

namespace webfuse
{

response_type get_response_type(request_type value)
{
    switch (value)
    {
        case request_type::access: return response_type::access;
        case request_type::getattr: return response_type::getattr;
        case request_type::readlink: return response_type::readlink;
        case request_type::symlink: return response_type::symlink;
        case request_type::link: return response_type::link;
        case request_type::rename: return response_type::rename;
        case request_type::chmod: return response_type::chmod;
        case request_type::chown: return response_type::chown;
        case request_type::truncate: return response_type::truncate;
        case request_type::fsync: return response_type::fsync;
        case request_type::open: return response_type::open;
        case request_type::mknod: return response_type::mknod;
        case request_type::create: return response_type::create;
        case request_type::release: return response_type::release;
        case request_type::unlink: return response_type::unlink;
        case request_type::read: return response_type::read;
        case request_type::write: return response_type::write;
        case request_type::mkdir: return response_type::mkdir;
        case request_type::readdir: return response_type::readdir;
        case request_type::rmdir: return response_type::rmdir;
        case request_type::statfs: return response_type::statfs;
        case request_type::utimens: return response_type::utimens;
        case request_type::getcreds: return response_type::getcreds;
        default:
            return response_type::unknown;
    }
}


}