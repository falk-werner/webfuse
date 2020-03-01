#ifndef WF_JSONRPC_IMPL_PROXY_H
#define WF_JSONRPC_IMPL_PROXY_H

#include "wf/jsonrpc/proxy_finished_fn.h"
#include "wf/jsonrpc/send_fn.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_timer_manager;
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
wf_jsonrpc_impl_proxy_init(
    struct wf_jsonrpc_proxy * proxy,
    struct wf_timer_manager * manager,
    int timeout,
    wf_jsonrpc_send_fn * send,
    void * user_data);

extern void 
wf_jsonrpc_impl_proxy_cleanup(
    struct wf_jsonrpc_proxy * proxy);

extern struct wf_jsonrpc_proxy *
wf_jsonrpc_impl_proxy_create(
    struct wf_timer_manager * manager,
    int timeout,
    wf_jsonrpc_send_fn * send,
    void * user_data);

extern void
wf_jsonrpc_impl_proxy_dispose(
    struct wf_jsonrpc_proxy * proxy);


extern void
wf_jsonrpc_impl_proxy_invoke(
	struct wf_jsonrpc_proxy * proxy,
	wf_jsonrpc_proxy_finished_fn * finished,
	void * user_data,
	char const * method_name,
	char const * param_info,
	va_list args
);

extern void
wf_jsonrpc_impl_proxy_notify(
	struct wf_jsonrpc_proxy * proxy,
	char const * method_name,
	char const * param_info,
	va_list args
);

extern void
wf_jsonrpc_impl_proxy_onresult(
    struct wf_jsonrpc_proxy * proxy,
    json_t * message);


#ifdef __cplusplus
}
#endif

#endif
