#ifndef WEBFUSE_MESSAGEBUILDER_HPP
#define WEBFUSE_MESSAGEBUILDER_HPP

#include "webfuse/message_type.hpp"

#include <cinttypes>
#include <string>
#include <vector>

namespace webfuse
{

class message
{
    message(message const &) = delete;
    message& operator=(message const &) = delete;
public:
    explicit message(message_type msg_type);
    ~message() = default;
    message(message && other);
    message& operator=(message && other);

    void set_id(uint32_t value);
    uint32_t get_id() const;

    void add_bool(bool value);
    void add_u8(uint8_t value);
    void add_i8(int8_t value);
    void add_i32(int32_t value);
    void add_u32(uint32_t value);
    void add_u64(uint64_t value);
    void add_str(std::string const &value);
    void add_data(char const * buffer, size_t size);
    void add_strings(std::vector<std::string> const & list);

    unsigned char * get_data(size_t &size);

private:
    uint32_t id;
    std::vector<uint8_t> data;
};

}

#endif
