#include "webfuse/ws/messagewriter.hpp"
#include "webfuse/filesystem/accessmode.hpp"
#include "webfuse/filesystem/filemode.hpp"
#include "webfuse/filesystem/openflags.hpp"

#include <libwebsockets.h>

namespace webfuse
{

constexpr uint8_t const rename_noreplace = 0x01;
constexpr uint8_t const rename_exchange  = 0x02;

messagewriter::messagewriter(message_type msg_type)
: id(0)
, data(LWS_PRE)
{
    write_u32(0);
    write_u8(static_cast<uint8_t>(msg_type));
}

messagewriter::messagewriter(messagewriter && other)
{
    this->id = other.id;
    this->data = std::move(other.data);
}

messagewriter& messagewriter::operator=(messagewriter && other)
{
    if (this != &other)
    {
        this->id = other.id;
        this->data = std::move(other.data);
    }

    return *this;
}

void messagewriter::set_id(uint32_t value)
{
    id = value;
    data[LWS_PRE    ] = (id >> 24) & 0xff;
    data[LWS_PRE + 1] = (id >> 16) & 0xff;
    data[LWS_PRE + 2] = (id >>  8) & 0xff;
    data[LWS_PRE + 3] =  id        & 0xff;
}

uint32_t messagewriter::get_id() const
{
    return id;
}

void messagewriter::write_bool(bool value)
{
    data.push_back(value ? 0x01 : 0x00);
}

void messagewriter::write_u8(uint8_t value)
{
    data.push_back(value);
}

void messagewriter::write_i8(int8_t value)
{
    data.push_back(static_cast<uint8_t>(value));
}

void messagewriter::write_i32(int32_t value)
{
    write_u32((static_cast<uint32_t>(value)));
}

void messagewriter::write_u32(uint32_t value)
{
    auto const offset = data.size(); 
    data.resize(offset + 4);
    data[offset    ] = (value >> 24) & 0xff;
    data[offset + 1] = (value >> 16) & 0xff;
    data[offset + 2] = (value >>  8) & 0xff;
    data[offset + 3] =  value        & 0xff;
}

void messagewriter::write_u64(uint64_t value)
{
    auto const offset = data.size(); 
    data.resize(offset + 8);
    data[offset    ] = (value >> 56) & 0xff;
    data[offset + 1] = (value >> 48) & 0xff;
    data[offset + 2] = (value >> 40) & 0xff;
    data[offset + 3] = (value >> 32) & 0xff;
    data[offset + 4] = (value >> 24) & 0xff;
    data[offset + 5] = (value >> 16) & 0xff;
    data[offset + 6] = (value >>  8) & 0xff;
    data[offset + 7] =  value        & 0xff;
}

void messagewriter::write_str(std::string const &value)
{
    write_data(value.data(), value.size());
}

void messagewriter::write_data(char const * buffer, size_t size)
{
    uint32_t const effective_size = size & 0xffffffff;
    write_u32(effective_size);

    if (size > 0)
    {
        auto const offset = data.size();
        data.resize(offset + effective_size);
        void * to = reinterpret_cast<void*>(&data.data()[offset]);
        void const * from = reinterpret_cast<void const *>(buffer);
        memcpy(to, from, effective_size);
    }
}

void messagewriter::write_strings(std::vector<std::string> const & list)
{
    uint32_t const count = list.size() & 0xffffffff;
    write_u32(count);
    for (auto const & item: list)
    {
        write_str(item);
    }
}

void messagewriter::write_access_mode(int value)
{
    access_mode mode = access_mode::from_int(value);
    write_i8(mode);
}

void messagewriter::write_rename_flags(unsigned int value)
{
    uint8_t flags = 0;
    if (RENAME_NOREPLACE == (value & RENAME_NOREPLACE)) { flags |= rename_noreplace; }
    if (RENAME_EXCHANGE  == (value & RENAME_EXCHANGE )) { flags |= rename_exchange;  }

    write_u8(flags);
}

void messagewriter::write_mode(mode_t value)
{
    filemode mode = filemode::from_mode(value);
    write_u32(mode);
}

void messagewriter::write_uid(uid_t value)
{
    write_u32(static_cast<uint32_t>(value));
}

void messagewriter::write_gid(gid_t value)
{
    write_u32(static_cast<uint32_t>(value));
}

void messagewriter::write_openflags(int value)
{
    openflags flags = openflags::from_int(value);
    write_i32(flags);
}

void messagewriter::write_time(timespec const & value)
{
    write_u64(static_cast<uint64_t>(value.tv_sec));
    write_u32(static_cast<uint32_t>(value.tv_nsec));
}


unsigned char * messagewriter::get_data(size_t &size)
{
    size = data.size() - LWS_PRE;
    void * result = reinterpret_cast<void *>(&data.data()[LWS_PRE]);

    return reinterpret_cast<unsigned char *>(result);
}


}