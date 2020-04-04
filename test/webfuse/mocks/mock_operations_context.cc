#include "webfuse/mocks/mock_operations_context.hpp"
#include "webfuse/utils/wrap.hpp"

extern "C"
{
static webfuse_test::MockOperationsContext * webfuse_test_MockOperationsContext = nullptr;

WF_WRAP_FUNC1(webfuse_test_MockOperationsContext, 
    struct wf_jsonrpc_proxy *,  wf_impl_operations_context_get_proxy,
	struct wf_impl_operations_context *);

}

namespace webfuse_test
{

MockOperationsContext::MockOperationsContext()
{
    webfuse_test_MockOperationsContext = this;
}

MockOperationsContext::~MockOperationsContext()
{
    webfuse_test_MockOperationsContext = nullptr;
}

}