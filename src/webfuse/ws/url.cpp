#include "webfuse/ws/url.hpp"

#include <cstring>
#include <stdexcept>

namespace
{

bool starts_with(std::string const & value, std::string const & prefix)
{
    return (0 == value.find(prefix));
}

std::string parse_protocol(std::string const &url, bool &use_tls)
{
    if (starts_with(url, "ws://"))
    {
        use_tls = false;
        return url.substr(strlen("ws://"));
    }
    
    if (starts_with(url, "wss://"))
    {
        use_tls = true;
        return url.substr(strlen("wss://"));
    }

    throw std::runtime_error("unknown protocol");
}


}

namespace webfuse
{

constexpr uint16_t const ws_port = 80;
constexpr uint16_t const wss_port = 443;

ws_url::ws_url(std::string const & url)
{
    auto remainder = parse_protocol(url, use_tls);

    auto const path_start = remainder.find('/');
    if (path_start != std::string::npos)
    {
        path = remainder.substr(path_start);
        remainder = remainder.substr(0, path_start);
    }
    else
    {
        path = "/";
    }

    auto const port_start = remainder.find(':');
    if (port_start != std::string::npos)
    {
        auto const port_str = remainder.substr(port_start + 1);
        port = static_cast<uint16_t>(std::stoi(port_str));
        hostname = remainder.substr(0, port_start);
    }
    else
    {
        port = (use_tls) ? wss_port : ws_port;
        hostname = remainder;
    }
}


}