#ifndef WEBFUSE_WSCLIENT_HPP
#define WEBFUSE_WSCLIENT_HPP

#include "webfuse/ws/messagewriter.hpp"
#include "webfuse/ws/messagereader.hpp"

#include <functional>

namespace webfuse
{

using ws_client_handler = std::function<messagewriter (messagereader & reader)>;

class ws_client
{
    ws_client(ws_client const &) = delete;
    ws_client& operator=(ws_client const &) = delete;
public:
    ws_client(ws_client_handler handler);
    ~ws_client();
    ws_client(ws_client && other);
    ws_client& operator=(ws_client && other);

    void set_connection_listener(std::function<void(bool)> listener);
    void connect(std::string url);
    void service();
    void interrupt();
private:
    class detail;
    detail * d;
};

}

#endif
