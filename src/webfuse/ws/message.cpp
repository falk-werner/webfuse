#include "message.hpp"
#include <libwebsockets.h>

namespace webfuse
{

message::message(message_type msg_type)
: id(0)
, data(LWS_PRE)
{
    add_u32(0);
    add_u8(static_cast<uint8_t>(msg_type));
}

message::message(message && other)
{
    this->id = other.id;
    this->data = std::move(other.data);
}

message& message::operator=(message && other)
{
    if (this != &other)
    {
        this->id = other.id;
        this->data = std::move(other.data);
    }

    return *this;
}

void message::set_id(uint32_t value)
{
    id = id;
    data[LWS_PRE    ] = (id >> 24) & 0xff;
    data[LWS_PRE + 1] = (id >> 16) & 0xff;
    data[LWS_PRE + 2] = (id >>  8) & 0xff;
    data[LWS_PRE + 3] =  id        & 0xff;
}

uint32_t message::get_id() const
{
    return id;
}

void message::add_bool(bool value)
{
    data.push_back(value ? 0x01 : 0x00);
}

void message::add_u8(uint8_t value)
{
    data.push_back(value);
}

void message::add_i8(int8_t value)
{
    data.push_back(static_cast<uint8_t>(value));
}

void message::add_i32(int32_t value)
{
    add_u32((static_cast<uint32_t>(value)));
}

void message::add_u32(uint32_t value)
{
    auto const offset = data.size(); 
    data.resize(offset + 4);
    data[offset    ] = (value >> 24) & 0xff;
    data[offset + 1] = (value >> 16) & 0xff;
    data[offset + 2] = (value >>  8) & 0xff;
    data[offset + 3] =  value        & 0xff;
}

void message::add_u64(uint64_t value)
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

void message::add_str(std::string const &value)
{
    add_data(value.data(), value.size());
}

void message::add_data(char const * buffer, size_t size)
{
    uint32_t const effective_size = size & 0xffffffff;
    add_u32(effective_size);

    if (size > 0)
    {
        auto const offset = data.size();
        data.resize(offset + effective_size);
        void * to = reinterpret_cast<void*>(&data.data()[offset]);
        void const * from = reinterpret_cast<void const *>(buffer);
        memcpy(to, from, effective_size);
    }
}

void message::add_strings(std::vector<std::string> const & list)
{
    uint32_t const count = list.size() & 0xffffffff;
    add_u32(count);
    for (auto const & item: list)
    {
        add_str(item);
    }
}

unsigned char * message::get_data(size_t &size)
{
    size = data.size() - LWS_PRE;
    void * result = reinterpret_cast<void *>(&data.data()[LWS_PRE]);

    return reinterpret_cast<unsigned char *>(result);
}


}