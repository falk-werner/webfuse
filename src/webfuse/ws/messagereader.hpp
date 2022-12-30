#ifndef WEBFUSE_MESSAGEREADER_HPP
#define WEBFUSE_MESSAGEREADER_HPP

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <unistd.h>

#include <cinttypes>
#include <string>
#include <vector>

namespace webfuse
{

class messagereader
{
    messagereader(messagereader const &) = delete;
    messagereader& operator=(messagereader const &) = delete;
public:
    explicit messagereader(std::string & value);
    ~messagereader() = default;
    messagereader(messagereader && other);
    messagereader& operator=(messagereader && other);

    int read_result();
    void read_attr(struct stat * attr);
    void read_statistics(struct statvfs * statistics);
    int read_access_mode();
    mode_t read_mode();

    uint8_t read_u8();
    uint32_t read_u32();
    uint64_t read_u64();

    int32_t read_i32();

    std::string read_str();
    std::string read_bytes();

    void read_strings(std::vector<std::string> &entries);



private:
    std::string data;
    size_t pos;
};

}

#endif
