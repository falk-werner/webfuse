#ifndef WF_TEST_INTEGRATION_SERVER_HPP
#define WF_TEST_INTEGRATION_SERVER_HPP

namespace webfuse_test
{

class Server
{
public:
    Server();
    ~Server();
    void Start(void);
    void Stop(void);
    char const * GetBaseDir(void) const;
private:
    class Private;
    Private * d;
};

}

#endif
