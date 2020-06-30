#ifndef WF_TEST_UTIL_WS_CLIENT_HPP
#define WF_TEST_UTIL_WS_CLIENT_HPP

#include <string>
#include <future>

namespace webfuse_test
{

class InvokationHandler;

class WsClient
{
    WsClient(WsClient const &) = delete;
    WsClient & operator=(WsClient const &) = delete;
public:
    WsClient(
        InvokationHandler& handler,
        std::string const & protocol);
    virtual ~WsClient();
    std::future<void> Connect(int port, std::string const & protocol, bool use_tls = true);
    std::future<std::string> Invoke(std::string const & message);
private:
    class Private;
    Private *d;
};

}

#endif
