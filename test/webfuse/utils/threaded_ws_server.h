#ifndef WF_TEST_UTILS_THREADED_WS_SERVER_HPP
#define WF_TEST_UTILS_THREADED_WS_SERVER_HPP

#include <libwebsockets.h>
#include <jansson.h>
#include <string>

namespace webfuse_test
{

class ThreadedWsServer
{
    ThreadedWsServer(ThreadedWsServer const &) = delete;
    ThreadedWsServer & operator=(ThreadedWsServer const &) = delete;
public:
    ThreadedWsServer(std::string const & protocol, int port = 0);
    ~ThreadedWsServer();
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
