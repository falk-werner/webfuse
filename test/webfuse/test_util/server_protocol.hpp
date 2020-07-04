#ifndef WF_TEST_UTIL_SERVER_PROTOCOL_HPP
#define WF_TEST_UTIL_SERVER_PROTOCOL_HPP

namespace webfuse_test
{

class ServerProtocol
{
public:
    ServerProtocol();
    ~ServerProtocol();
    char const * GetBaseDir() const;
    int GetPort() const;
private:
    class Private;
    Private * d;
};

}


#endif
