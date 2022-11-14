#ifndef WEBFUSE_WSSERVER_HPP
#define WEBFUSE_WSSERVER_HPP

#include "webfuse/ws/config.hpp"
#include <future>
#include <string>
namespace webfuse
{

class ws_server
{
    ws_server(ws_server const &) = delete;
    ws_server& operator=(ws_server const &) = delete;
public:
    ws_server(ws_config const & config);
    ~ws_server();
    ws_server(ws_server && other);
    ws_server& operator=(ws_server && other);
    std::future<std::string> perform(std::string const & req);
private:
    class detail;
    detail * d;
};

}

#endif
