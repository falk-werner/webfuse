#include "webfuse/filesystem/groupid.hpp"

namespace webfuse
{

group_id::group_id(uint32_t value)
: value_(value)
{

}

group_id::operator uint32_t() const
{
    return value_;
}

group_id group_id::from_gid(gid_t value)
{
    return group_id(static_cast<uint32_t>(value));
}

gid_t group_id::to_gid() const
{
    return static_cast<gid_t>(value_);
}

}