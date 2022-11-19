#ifndef WEBFUSE_MESSAGEREADER_HPP
#define WEBFUSE_MESSAGEREADER_HPP

#include <string>

namespace webfuse
{

class messagereader
{

public:
    explicit messagereader(std::string && value);
    ~messagereader() = default;

private:
    std::string data;
    size_t pos;
};

}

#endif
