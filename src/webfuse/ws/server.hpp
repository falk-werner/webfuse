#ifndef WEBFUSE_WSSERVER_HPP
#define WEBFUSE_WSSERBER_HPP

#include "webfuse/ws/config.hpp"

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
    void service();
    void interrupt();
private:
    class detail;
    detail * d;
};

}

#endif
