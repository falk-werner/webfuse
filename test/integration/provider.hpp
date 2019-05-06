#ifndef WF_TEST_INTEGRATION_PROVIDER
#define WF_TEST_INTEGRATION_PROVIDER

namespace webfuse_test
{

class Provider
{
public:
    explicit Provider(char const * url);
    ~Provider();
private:
    class Private;
    Private * d;
};

}

#endif
