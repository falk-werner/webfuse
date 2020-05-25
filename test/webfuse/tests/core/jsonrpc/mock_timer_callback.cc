#include "webfuse/tests/core/jsonrpc/mock_timer_callback.hpp"

extern "C"
{
using wf_jsonrpc_test::MockTimerCallback;

static void wf_jsonrpc_test_MockTimerCallback_on_timer(
    wf_timer * timer,
    void * user_data)
{
    auto * self = reinterpret_cast<MockTimerCallback*>(user_data);
    self->on_timer(timer, user_data);
}

}

namespace wf_jsonrpc_test
{

MockTimerCallback::MockTimerCallback()
{

}

MockTimerCallback::~MockTimerCallback()
{

}

wf_timer_on_timer_fn * MockTimerCallback::on_timer_fn()
{
    return &wf_jsonrpc_test_MockTimerCallback_on_timer;
}

void * MockTimerCallback::user_data()
{
    return reinterpret_cast<void*>(this);
}


}