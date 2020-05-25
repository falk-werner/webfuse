#ifndef WF_MOCK_PROVIDER_HPP
#define WF_MOCK_PROVIDER_HPP

#include "webfuse/provider/impl/provider.h"
#include <gmock/gmock.h>

namespace webfuse_test
{

class MockProvider
{
public:
    MockProvider();
    ~MockProvider();
    wfp_provider const * get_provider();
    void * get_userdata();
    MOCK_METHOD0(connected, void ());
    MOCK_METHOD0(disconnected, void ());
    MOCK_METHOD0(on_timer, void());
    MOCK_METHOD3(lookup, void(wfp_request * request, ino_t parent, char const * name));
    MOCK_METHOD2(getattr, void(wfp_request * request, ino_t inode));
    MOCK_METHOD2(readdir, void(wfp_request * request, ino_t directory));
    MOCK_METHOD3(open, void(wfp_request * request, ino_t inode, int flags));
    MOCK_METHOD3(close, void(ino_t inode, uint32_t handle, int flags));
    MOCK_METHOD5(read, void(wfp_request * request, ino_t inode, uint32_t handle, size_t offset, size_t length));
    MOCK_METHOD1(get_credentials, void(wfp_credentials * credentials));
};

}

#endif
