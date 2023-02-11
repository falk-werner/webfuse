#include "webfuse/request_type.hpp"

namespace webfuse
{

request_type get_request_type(uint8_t value)
{
    switch (value)
    {
        case static_cast<uint8_t>(request_type::access): return request_type::access;
        case static_cast<uint8_t>(request_type::getattr): return request_type::getattr;
        case static_cast<uint8_t>(request_type::readlink): return request_type::readlink;
        case static_cast<uint8_t>(request_type::symlink): return request_type::symlink;
        case static_cast<uint8_t>(request_type::link): return request_type::link;
        case static_cast<uint8_t>(request_type::rename): return request_type::rename;
        case static_cast<uint8_t>(request_type::chmod): return request_type::chmod;
        case static_cast<uint8_t>(request_type::chown): return request_type::chown;
        case static_cast<uint8_t>(request_type::truncate): return request_type::truncate;
        case static_cast<uint8_t>(request_type::fsync): return request_type::fsync;
        case static_cast<uint8_t>(request_type::open): return request_type::open;
        case static_cast<uint8_t>(request_type::mknod): return request_type::mknod;
        case static_cast<uint8_t>(request_type::create): return request_type::create;
        case static_cast<uint8_t>(request_type::release): return request_type::release;
        case static_cast<uint8_t>(request_type::unlink): return request_type::unlink;
        case static_cast<uint8_t>(request_type::read): return request_type::read;
        case static_cast<uint8_t>(request_type::write): return request_type::write;
        case static_cast<uint8_t>(request_type::mkdir): return request_type::mkdir;
        case static_cast<uint8_t>(request_type::readdir): return request_type::readdir;
        case static_cast<uint8_t>(request_type::rmdir): return request_type::rmdir;
        case static_cast<uint8_t>(request_type::statfs): return request_type::statfs;
        case static_cast<uint8_t>(request_type::utimens): return request_type::utimens;
        case static_cast<uint8_t>(request_type::getcreds): return request_type::getcreds;
        default:
            return request_type::unknown;
    }
}

}
