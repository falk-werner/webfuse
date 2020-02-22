#ifndef WF_TEST_UTILS_WS_SERVER_HPP
#define WF_TEST_UTILS_WS_SERVER_HPP

#include <libwebsockets.h>
#include <jansson.h>

namespace webfuse_test
{

class WebsocketServer
{
    WebsocketServer(WebsocketServer const &) = delete;
    WebsocketServer & operator=(WebsocketServer const &) = delete;
public:
    explicit WebsocketServer(int port);
    ~WebsocketServer();
    void waitForConnection();
    void sendMessage(json_t * message);
    json_t * receiveMessage();
private:
    class Private;
    Private * d;
};

}

#endif
