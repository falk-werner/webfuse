#ifndef WF_MOCK_ADAPTER_CLIENT_CALLBACK_HPP
#define WF_MOCK_ADAPTER_CLIENT_CALLBACK_HPP

#include <gmock/gmock.h>
#include "webfuse/adapter/client_callback.h"

namespace webfuse_test
{

class MockAdapterClientCallback
{
public:
    MockAdapterClientCallback();
    virtual ~MockAdapterClientCallback();
    MOCK_METHOD3(Invoke, void (wf_client *, int, void *));
    void * GetUserData();
    wf_client_callback_fn * GetCallbackFn();
};

}

#endif
