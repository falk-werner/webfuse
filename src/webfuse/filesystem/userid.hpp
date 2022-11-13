#ifndef WEBFUSE_USERID_HPP
#define WEBFUSE_USERID_HPP

#include <unistd.h>
#include <cinttypes>

namespace webfuse
{

class user_id
{
public:
    static constexpr uint32_t const invalid   = (uint32_t) -1;

    explicit user_id(uint32_t value = invalid);
    operator uint32_t() const;
    static user_id from_uid(uid_t value);
    uid_t to_uid() const;
private:
    uint32_t value_;
};

}

#endif
