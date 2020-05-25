#include "webfuse/tests/core/jsonrpc/mock_timer.hpp"
#include "webfuse/utils/wrap.hpp"

extern "C"
{
static wf_jsonrpc_test::ITimer * wf_jsonrpc_MockTimer = nullptr;

WF_WRAP_FUNC0(wf_jsonrpc_MockTimer, wf_timer_manager *, wf_timer_manager_create);
WF_WRAP_FUNC1(wf_jsonrpc_MockTimer, void, wf_timer_manager_dispose, wf_timer_manager *);
WF_WRAP_FUNC1(wf_jsonrpc_MockTimer, void, wf_timer_manager_check, wf_timer_manager *);

WF_WRAP_FUNC3(wf_jsonrpc_MockTimer, wf_timer *, wf_timer_create, wf_timer_manager *, wf_timer_on_timer_fn *, void *);
WF_WRAP_FUNC1(wf_jsonrpc_MockTimer, void, wf_timer_dispose, wf_timer *);
WF_WRAP_FUNC2(wf_jsonrpc_MockTimer, void, wf_timer_start, wf_timer *, int);
WF_WRAP_FUNC1(wf_jsonrpc_MockTimer, void, wf_timer_cancel, wf_timer *);

}

namespace wf_jsonrpc_test
{
MockTimer::MockTimer()
{
    wf_jsonrpc_MockTimer = this;
}

MockTimer::~MockTimer()
{
    wf_jsonrpc_MockTimer = nullptr;
}

}