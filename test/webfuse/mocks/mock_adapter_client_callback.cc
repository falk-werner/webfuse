#include "webfuse/mocks/mock_adapter_client_callback.hpp"
#include "webfuse/adapter/client.h"

extern "C"
{

static void
webfuse_test_MockAdapterClientCallback_callback(
    wf_client * client,
    int reason,
    void * args)
{
    void * user_data = wf_client_get_userdata(client);
    auto * callback = reinterpret_cast<webfuse_test::MockAdapterClientCallback*>(user_data);

    callback->Invoke(client, reason, args);
}

}

namespace webfuse_test
{

MockAdapterClientCallback::MockAdapterClientCallback()
{

}

MockAdapterClientCallback::~MockAdapterClientCallback()
{

}

void * MockAdapterClientCallback::GetUserData()
{
    return reinterpret_cast<void*>(this);
}

wf_client_callback_fn * MockAdapterClientCallback::GetCallbackFn()
{
    return &webfuse_test_MockAdapterClientCallback_callback;
}

}