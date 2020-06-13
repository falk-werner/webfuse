#ifndef WF_TEST_UTILS_WS_SERVER_HPP
#define WF_TEST_UTILS_WS_SERVER_HPP

#include <jansson.h>
#include <string>

namespace webfuse_test
{

class WsServer
{
    WsServer(WsServer const &) = delete;
    WsServer & operator=(WsServer const &) = delete;
public:
    WsServer(std::string const & protocol, int port = 0);
    ~WsServer();
    bool IsConnected();
    std::string GetUrl() const;
    void SendMessage(json_t * message);
    json_t * ReceiveMessage();
private:
    class Private;
    Private * d;
};

}


#endif
