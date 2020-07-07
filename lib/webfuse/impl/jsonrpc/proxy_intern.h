#ifndef WF_IMPL_JSONRPC_PROXY_INTERN_H
#define WF_IMPL_JSONRPC_PROXY_INTERN_H

#include "webfuse/impl/jsonrpc/proxy.h"
#include "webfuse/impl/jsonrpc/proxy_finished_fn.h"
#include "webfuse/impl/jsonrpc/send_fn.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_jsonrpc_proxy_request_manager;

struct wf_jsonrpc_proxy
{
    struct wf_jsonrpc_proxy_request_manager * request_manager;
    wf_jsonrpc_send_fn * send;
    void * user_data;
};

extern void 
wf_impl_jsonrpc_proxy_init(
    struct wf_jsonrpc_proxy * proxy,
    struct wf_timer_manager * manager,
    int timeout,
    wf_jsonrpc_send_fn * send,
    void * user_data);

extern void 
wf_impl_jsonrpc_proxy_cleanup(
    struct wf_jsonrpc_proxy * proxy);

extern void wf_impl_jsonrpc_proxy_vinvoke(
	struct wf_jsonrpc_proxy * proxy,
	wf_jsonrpc_proxy_finished_fn * finished,
	void * user_data,
	char const * method_name,
	char const * param_info,
	va_list args);

extern void wf_impl_jsonrpc_proxy_vnotify(
	struct wf_jsonrpc_proxy * proxy,
	char const * method_name,
	char const * param_info,
	va_list args);

#ifdef __cplusplus
}
#endif

#endif
