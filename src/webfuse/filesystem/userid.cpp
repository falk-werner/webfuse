#include "webfuse/filesystem/userid.hpp"

namespace webfuse
{

user_id::user_id(uint32_t value)
: value_(value)
{

}

user_id::operator uint32_t() const
{
    return value_;
}

user_id user_id::from_uid(uid_t value)
{
    return user_id(static_cast<uint32_t>(value));
}

uid_t user_id::to_uid() const
{
    return static_cast<uid_t>(value_);
}


}