#ifndef WEBFUSE_WSSERVER_HPP
#define WEBFUSE_WSSERVER_HPP

#include "webfuse/ws/config.hpp"
#include "webfuse/ws/messagewriter.hpp"
#include "webfuse/ws/messagereader.hpp"

#include <vector>
#include <string>
#include <memory>

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
    
    messagereader perform(messagewriter writer);
private:
    class detail;
    detail * d;
};

}

#endif
