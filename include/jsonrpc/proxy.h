#ifndef JSONRPC_PROXY_H
#define JSONRPC_PROXY_H

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
#include "jsonrpc/send_fn.h"
#include "webfuse/adapter/impl/time/timeout_manager.h"
#include "webfuse/adapter/impl/time/timer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void jsonrpc_proxy_finished_fn(
	void * user_data,
	json_t const * result,
    json_t const * error);


struct jsonrpc_request
{
    bool is_pending;
    jsonrpc_proxy_finished_fn * finished;
    void * user_data;
    int id;
    struct wf_impl_timer timer;
};

struct jsonrpc_proxy
{
    struct jsonrpc_request request;
    int timeout;
    jsonrpc_send_fn * send;
    void * user_data;
};

extern void jsonrpc_proxy_init(
    struct jsonrpc_proxy * proxy,
    struct wf_impl_timeout_manager * manager,
    int timeout,
    jsonrpc_send_fn * send,
    void * user_data);

extern void jsonrpc_proxy_cleanup(
    struct jsonrpc_proxy * proxy);

extern void jsonrpc_proxy_invoke(
	struct jsonrpc_proxy * proxy,
	jsonrpc_proxy_finished_fn * finished,
	void * user_data,
	char const * method_name,
	char const * param_info,
	...
);

extern void jsonrpc_proxy_notify(
	struct jsonrpc_proxy * proxy,
	char const * method_name,
	char const * param_info,
	...
);

extern void jsonrpc_proxy_onresult(
    struct jsonrpc_proxy * proxy,
    json_t * message);

#ifdef __cplusplus
}
#endif


#endif
