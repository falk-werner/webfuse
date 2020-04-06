#include "webfuse/core/jsonrpc/proxy_intern.h"

void wf_jsonrpc_proxy_invoke(
	struct wf_jsonrpc_proxy * proxy,
	wf_jsonrpc_proxy_finished_fn * finished,
	void * user_data,
	char const * method_name,
	char const * param_info,
	...)
{
    va_list args;
    va_start(args, param_info);
    wf_jsonrpc_proxy_vinvoke(proxy, finished, user_data, method_name, param_info, args);
    va_end(args);
}

extern void wf_jsonrpc_proxy_notify(
	struct wf_jsonrpc_proxy * proxy,
	char const * method_name,
	char const * param_info,
	...
)
{
    va_list args;
    va_start(args, param_info);
    wf_jsonrpc_proxy_vnotify(proxy, method_name, param_info, args);
    va_end(args);
}
