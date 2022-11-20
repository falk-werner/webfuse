#ifndef WEBFUSE_MESSAGEWRITER_HPP
#define WEBFUSE_MESSAGEWRITER_HPP

#include "webfuse/message_type.hpp"

#include <cinttypes>
#include <string>
#include <vector>

namespace webfuse
{

class messagewriter
{
    messagewriter(messagewriter const &) = delete;
    messagewriter& operator=(messagewriter const &) = delete;
public:
    explicit messagewriter(message_type msg_type);
    ~messagewriter() = default;
    messagewriter(messagewriter && other);
    messagewriter& operator=(messagewriter && other);

    void set_id(uint32_t value);
    uint32_t get_id() const;

    void write_bool(bool value);
    void write_u8(uint8_t value);
    void write_i8(int8_t value);
    void write_i32(int32_t value);
    void write_u32(uint32_t value);
    void write_u64(uint64_t value);
    void write_str(std::string const &value);
    void write_data(char const * buffer, size_t size);
    void write_strings(std::vector<std::string> const & list);

    void write_access_mode(int value);
    void write_rename_flags(unsigned int value);
    void write_mode(mode_t value);
    void write_uid(uid_t value);
    void write_gid(gid_t value);
    void write_openflags(int value);

    unsigned char * get_data(size_t &size);

private:
    uint32_t id;
    std::vector<uint8_t> data;
};

}

#endif
