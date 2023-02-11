#ifndef WEBFUSE_URL_HPP
#define WEBFUSE_URL_HPP

#include <cinttypes>
#include <string>

namespace webfuse
{

class ws_url
{
public:
    ws_url(std::string const & url);
    ~ws_url() = default;

    bool use_tls;
    std::string hostname;
    uint16_t port;
    std::string path;
};

}

#endif
