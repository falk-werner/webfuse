#include "webfuse/mocks/mock_jsonrpc_proxy.hpp"
#include "webfuse/utils/wrap.hpp"

extern "C"
{
static webfuse_test::MockJsonRpcProxy * webfuse_test_MockJsonRpcProxy = nullptr;

WF_WRAP_VFUNC5(webfuse_test_MockJsonRpcProxy, void, wf_jsonrpc_proxy_vinvoke,
	struct wf_jsonrpc_proxy *,
	wf_jsonrpc_proxy_finished_fn *,
	void *,
	char const *,
	char const *);

WF_WRAP_VFUNC3(webfuse_test_MockJsonRpcProxy, void, wf_jsonrpc_proxy_vnotify,
	struct wf_jsonrpc_proxy *,
	char const *,
	char const *);
}

namespace webfuse_test
{

MockJsonRpcProxy::MockJsonRpcProxy()
{
    webfuse_test_MockJsonRpcProxy = this;
}

MockJsonRpcProxy::~MockJsonRpcProxy()
{
    webfuse_test_MockJsonRpcProxy = nullptr;
}

}