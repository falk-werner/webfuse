#include "webfuse/mocks/mock_operation_context.hpp"
#include "webfuse/utils/wrap.hpp"

extern "C"
{
static webfuse_test::MockOperationContext * webfuse_test_MockOperationContext = nullptr;

WF_WRAP_FUNC1(webfuse_test_MockOperationContext, 
    struct wf_jsonrpc_proxy *,  wf_impl_operation_context_get_proxy,
	struct wf_impl_operation_context *);

}

namespace webfuse_test
{

MockOperationContext::MockOperationContext()
{
    webfuse_test_MockOperationContext = this;
}

MockOperationContext::~MockOperationContext()
{
    webfuse_test_MockOperationContext = nullptr;
}

}