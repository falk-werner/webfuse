#include "webfuse/ws/messagereader.hpp"
#include "webfuse/filesystem/status.hpp"
#include "webfuse/filesystem/filemode.hpp"
#include "webfuse/filesystem/accessmode.hpp"

#include <stdexcept>

namespace webfuse
{

messagereader::messagereader(std::string & value)
: data(std::move(value))
, pos(0)
{
}

messagereader::messagereader(messagereader && other)
{
    this->data = std::move(other.data);
    this->pos = other.pos;
}

messagereader& messagereader::operator=(messagereader && other)
{
    if (this != &other)
    {
        this->data = std::move(other.data);
        this->pos = other.pos;
    }

    return *this;
}


int messagereader::read_result()
{
    status value(read_i32());
    return value.to_fusestatus();
}

int messagereader::read_access_mode()
{
    auto const value = read_u8();
    access_mode mode(static_cast<int8_t>(value));

    return mode.to_int();
}

void messagereader::read_attr(struct stat * attr)
{
    attr->st_ino = static_cast<ino_t>(read_u64());
    attr->st_nlink = static_cast<nlink_t>(read_u64());
    attr->st_mode = read_mode();
    attr->st_uid = static_cast<uid_t>(read_u32());
    attr->st_gid = static_cast<gid_t>(read_u32());
    attr->st_rdev = static_cast<dev_t>(read_u64());
    attr->st_size = static_cast<off_t>(read_u64());
    attr->st_blocks = static_cast<blkcnt64_t>(read_u64());
    attr->st_atim.tv_sec = static_cast<time_t>(read_u64());
    attr->st_atim.tv_nsec = static_cast<long>(read_u32());
    attr->st_mtim.tv_sec = static_cast<time_t>(read_u64());
    attr->st_mtim.tv_nsec = static_cast<long>(read_u32());
    attr->st_ctim.tv_sec = static_cast<time_t>(read_u64());
    attr->st_ctim.tv_nsec = static_cast<long>(read_u32());
}

void messagereader::read_statistics(struct statvfs * statistics)
{
    statistics->f_bsize = read_u64();
    statistics->f_frsize = read_u64();
    statistics->f_blocks = read_u64();
    statistics->f_bfree = read_u64();
    statistics->f_bavail = read_u64();
    statistics->f_files = read_u64();
    statistics->f_ffree = read_u64();
    statistics->f_namemax = read_u64();
}


mode_t messagereader::read_mode()
{
    filemode mode(read_u32());
    return mode.to_mode();
}

bool messagereader::read_bool()
{
    return (1 == read_u8());
}


uint8_t messagereader::read_u8()
{
    if (pos < data.size())
    {
        uint8_t value = static_cast<uint8_t>(data[pos]);
        pos++;
        return value;
    }
    else
    {
        throw std::runtime_error("out of bounds");
    }
}

uint32_t messagereader::read_u32()
{
    if ((pos + 3) < data.size())
    {
        uint32_t value = 
            ((static_cast<uint32_t>(data[pos    ]) & 0xff) << 24) |
            ((static_cast<uint32_t>(data[pos + 1]) & 0xff) << 16) |
            ((static_cast<uint32_t>(data[pos + 2]) & 0xff) <<  8) |
            ((static_cast<uint32_t>(data[pos + 3]) & 0xff)      );
        pos += 4;
        return value;
    }
    else
    {
        throw std::runtime_error("out of bounds");
    }
}

uint64_t messagereader::read_u64()
{
    if ((pos + 7) < data.size())
    {
        uint32_t value = 
            (static_cast<uint64_t>(data[pos    ] & 0xff) << 56) |
            (static_cast<uint64_t>(data[pos + 1] & 0xff) << 48) |
            (static_cast<uint64_t>(data[pos + 2] & 0xff) << 40) |
            (static_cast<uint64_t>(data[pos + 3] & 0xff) << 32) |
            (static_cast<uint64_t>(data[pos + 4] & 0xff) << 24) |
            (static_cast<uint64_t>(data[pos + 5] & 0xff) << 16) |
            (static_cast<uint64_t>(data[pos + 6] & 0xff) <<  8) |
            (static_cast<uint64_t>(data[pos + 7] & 0xff)      );
        pos += 8;
        return value;
    }
    else
    {
        throw std::runtime_error("out of bounds");
    }
}


int32_t messagereader::read_i32()
{
    uint32_t value = read_u32();
    return static_cast<int32_t>(value);
}

std::string messagereader::read_str()
{
    return std::move(read_bytes());
}

std::string messagereader::read_bytes()
{
    uint32_t size = read_u32();
    if ((pos + size) <= data.size())
    {
        std::string const value(&data[pos], size);
        pos += size;
        return std::move(value);
    }
    else
    {
        throw std::runtime_error("out of bounds");
    }
}

void messagereader::read_strings(std::vector<std::string> &entries)
{
    uint32_t const count = read_u32();
    for(uint32_t i = 0; i < count; i++)
    {
        std::string entry = read_str();
        entries.push_back(entry);
    } 
}


}