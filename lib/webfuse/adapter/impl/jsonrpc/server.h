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
#include "webfuse/adapter/impl/jsonrpc/method.h"
#include "webfuse/adapter/impl/time/timeout_manager.h"
#include "webfuse/adapter/impl/time/timer.h"

#ifdef __cplusplus
extern "C" {
#endif

struct wf_impl_jsonrpc_request
{
    bool is_pending;
    wf_impl_jsonrpc_method_finished_fn * finished;
    void * user_data;
    int id;
    struct wf_impl_timer timer;
};

struct wf_impl_jsonrpc_server
{
    struct wf_impl_jsonrpc_method * methods;
    struct wf_impl_jsonrpc_request request;
};

extern void wf_impl_jsonrpc_server_init(
    struct wf_impl_jsonrpc_server * server,
    struct wf_impl_timeout_manager * manager);

extern void wf_impl_jsonrpc_server_cleanup(
    struct wf_impl_jsonrpc_server * server);

extern void wf_impl_jsonrpc_server_add(
    struct wf_impl_jsonrpc_server * server,
    char const * name,
    wf_impl_jsonrpc_method_invoke_fn * invoke,
    void * user_data );

extern void wf_impl_jsonrpc_server_invoke(
	struct wf_impl_jsonrpc_server * server,
	wf_impl_jsonrpc_method_finished_fn * finished,
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
