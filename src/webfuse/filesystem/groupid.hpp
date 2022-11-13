#ifndef WEBFUSE_GROUPID_HPP
#define WEBFUSE_GROUPID_HPP

#include <unistd.h>
#include <cinttypes>

namespace webfuse
{

class group_id
{
public:
    static constexpr uint32_t const invalid = (uint32_t) -1;

    explicit group_id(uint32_t value = invalid);
    operator uint32_t() const;
    static group_id from_gid(gid_t value);
    gid_t to_gid() const;
private:
    uint32_t value_;
};

}

#endif
