#ifndef WF_TEST_UTILS_THREADED_WS_SERVER_HPP
#define WF_TEST_UTILS_THREADED_WS_SERVER_HPP

#include <libwebsockets.h>
#include <jansson.h>

namespace webfuse_test
{

class ThreadedWsServer
{
    ThreadedWsServer(ThreadedWsServer const &) = delete;
    ThreadedWsServer & operator=(ThreadedWsServer const &) = delete;
public:
    explicit ThreadedWsServer(int port);
    ~ThreadedWsServer();
    void WaitForConnection();
private:
    class Private;
    Private * d;
};

}


#endif
