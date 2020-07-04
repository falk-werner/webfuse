#ifndef WF_TEST_UTIL_WS_SERVER2_HPP
#define WF_TEST_UTIL_WS_SERVER2_HPP

#include <jansson.h>
#include <string>

namespace webfuse_test
{

class InvokationHandler;

class WsServer
{
    WsServer(WsServer const &) = delete;
    WsServer & operator=(WsServer const & ) = delete;
public:
    WsServer(
        InvokationHandler& handler,
        std::string const & protocol,
        int port = 0,
        bool enable_tls = false);
    virtual ~WsServer();
    std::string const & GetUrl() const;
    void SendMessage(char const * message);
    void SendMessage(json_t * message);
private:
    class Private;
    Private * d;
};

}

#endif
