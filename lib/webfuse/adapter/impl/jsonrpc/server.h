#ifndef WF_ADAPTER_IMPL_JSONRPC_SERVER_H
#define WF_ADAPTER_IMPL_JSONRPC_SERVER_H

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
#include "webfuse/adapter/impl/time/timeout_manager.h"
#include "webfuse/adapter/impl/time/timer.h"
#include "webfuse/core/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef bool wf_impl_jsonrpc_server_send_fn(
	json_t * request,
    void * user_data);


typedef void wf_impl_jsonrpc_server_finished_fn(
	void * user_data,
	wf_status status,
	struct json_t const * result);


struct wf_impl_jsonrpc_request
{
    bool is_pending;
    wf_impl_jsonrpc_server_finished_fn * finished;
    void * user_data;
    int id;
    struct wf_impl_timer timer;
};

struct wf_impl_jsonrpc_server
{
    struct wf_impl_jsonrpc_request request;
    wf_impl_jsonrpc_server_send_fn * send;
    void * user_data;
};

extern void wf_impl_jsonrpc_server_init(
    struct wf_impl_jsonrpc_server * server,
    struct wf_impl_timeout_manager * manager,
    wf_impl_jsonrpc_server_send_fn * send,
    void * user_data);

extern void wf_impl_jsonrpc_server_cleanup(
    struct wf_impl_jsonrpc_server * server);

extern void wf_impl_jsonrpc_server_invoke(
	struct wf_impl_jsonrpc_server * server,
	wf_impl_jsonrpc_server_finished_fn * finished,
	void * user_data,
	char const * method_name,
	char const * param_info,
	...
);

extern void wf_impl_jsonrpc_server_notify(
	struct wf_impl_jsonrpc_server * server,
	char const * method_name,
	char const * param_info,
	...
);

extern void wf_impl_jsonrpc_server_onresult(
    struct wf_impl_jsonrpc_server * server,
    char const * message,
    size_t length);

#ifdef __cplusplus
}
#endif


#endif
