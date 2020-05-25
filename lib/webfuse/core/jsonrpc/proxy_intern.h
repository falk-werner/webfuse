#ifndef WF_JSONRPC_PROXY_INTERN_H
#define WF_JSONRPC_PROXY_INTERN_H

#include "webfuse/core/jsonrpc/proxy.h"
#include "webfuse/core/jsonrpc/proxy_finished_fn.h"
#include "webfuse/core/jsonrpc/send_fn.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_timer;

struct wf_jsonrpc_request
{
    bool is_pending;
    wf_jsonrpc_proxy_finished_fn * finished;
    void * user_data;
    int id;
    struct wf_timer * timer;
};

struct wf_jsonrpc_proxy
{
    struct wf_jsonrpc_request request;
    int timeout;
    wf_jsonrpc_send_fn * send;
    void * user_data;
};

extern void 
wf_jsonrpc_proxy_init(
    struct wf_jsonrpc_proxy * proxy,
    struct wf_timer_manager * manager,
    int timeout,
    wf_jsonrpc_send_fn * send,
    void * user_data);

extern void 
wf_jsonrpc_proxy_cleanup(
    struct wf_jsonrpc_proxy * proxy);

extern void wf_jsonrpc_proxy_vinvoke(
	struct wf_jsonrpc_proxy * proxy,
	wf_jsonrpc_proxy_finished_fn * finished,
	void * user_data,
	char const * method_name,
	char const * param_info,
	va_list args);

extern void wf_jsonrpc_proxy_vnotify(
	struct wf_jsonrpc_proxy * proxy,
	char const * method_name,
	char const * param_info,
	va_list args);

#ifdef __cplusplus
}
#endif

#endif
