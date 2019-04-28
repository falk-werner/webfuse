#ifndef WF_ADAPTER_IMPL_JSONRPC_PROXY_H
#define WF_ADAPTER_IMPL_JSONRPC_PROXY_H

#ifndef __cplusplus
#include <stdarg.h>
#include <stddef.h>
#include <stdbool.h>
#else
#include <cstdarg>
#include <cstddef>
using std::size_t;
#endif

#include <jansson.h>
#include "webfuse/adapter/impl/jsonrpc/send_fn.h"
#include "webfuse/adapter/impl/time/timeout_manager.h"
#include "webfuse/adapter/impl/time/timer.h"
#include "webfuse/core/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void wf_impl_jsonrpc_proxy_finished_fn(
	void * user_data,
	wf_status status,
	struct json_t const * result);


struct wf_impl_jsonrpc_request
{
    bool is_pending;
    wf_impl_jsonrpc_proxy_finished_fn * finished;
    void * user_data;
    int id;
    struct wf_impl_timer timer;
};

struct wf_impl_jsonrpc_proxy
{
    struct wf_impl_jsonrpc_request request;
    int timeout;
    wf_impl_jsonrpc_send_fn * send;
    void * user_data;
};

extern void wf_impl_jsonrpc_proxy_init(
    struct wf_impl_jsonrpc_proxy * proxy,
    struct wf_impl_timeout_manager * manager,
    int timeout,
    wf_impl_jsonrpc_send_fn * send,
    void * user_data);

extern void wf_impl_jsonrpc_proxy_cleanup(
    struct wf_impl_jsonrpc_proxy * proxy);

extern void wf_impl_jsonrpc_proxy_invoke(
	struct wf_impl_jsonrpc_proxy * proxy,
	wf_impl_jsonrpc_proxy_finished_fn * finished,
	void * user_data,
	char const * method_name,
	char const * param_info,
	...
);

extern void wf_impl_jsonrpc_proxy_notify(
	struct wf_impl_jsonrpc_proxy * proxy,
	char const * method_name,
	char const * param_info,
	...
);

extern void wf_impl_jsonrpc_proxy_onresult(
    struct wf_impl_jsonrpc_proxy * proxy,
    json_t * message);

#ifdef __cplusplus
}
#endif


#endif
