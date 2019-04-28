#ifndef WF_TEST_INTEGRATION_PROVIDER
#define WF_TEST_INTEGRATION_PROVIDER

namespace webfuse_test
{

class Provider
{
public:
    Provider(char const * url);
    ~Provider();
    void Start(void);
    void Stop(void);
private:
    class Private;
    Private * d;
};

}

#endif
