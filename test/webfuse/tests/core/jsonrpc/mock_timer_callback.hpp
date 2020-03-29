#ifndef WF_JSONRPC_MOCK_TIMERCALLBACK_HPP
#define WF_JSONRPC_MOCK_TIMERCALLBACK_HPP

#include "webfuse/core/timer/on_timer_fn.h"
#include <gmock/gmock.h>

namespace wf_jsonrpc_test
{
class MockTimerCallback
{
public:
    MockTimerCallback();
    virtual ~MockTimerCallback();
    wf_timer_on_timer_fn * on_timer_fn();
    void * user_data();

    MOCK_METHOD2(on_timer, void (wf_timer * timer, void * user_data));

};

}

#endif
