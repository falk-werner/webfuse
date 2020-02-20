#ifndef WF_TEST_FAKE_SERVER_HPP
#define WF_TEST_FAKE_SERVER_HPP

#include <libwebsockets.h>

namespace webfuse_test
{

class FakeAdapterServer
{
    FakeAdapterServer(FakeAdapterServer const &) = delete;
    FakeAdapterServer & operator=(FakeAdapterServer const &) = delete;
public:
    explicit FakeAdapterServer(int port);
    ~FakeAdapterServer();
    void waitForConnection();
private:
    class Private;
    Private * d;
};

}

#endif
