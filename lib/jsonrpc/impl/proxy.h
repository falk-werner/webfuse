#ifndef JSONRPC_IMPL_PROXY_H
#define JSONRPC_IMPL_PROXY_H

#include "jsonrpc/proxy_finished_fn.h"
#include "jsonrpc/send_fn.h"

#include "webfuse/adapter/impl/time/timeout_manager.h"
#include "webfuse/adapter/impl/time/timer.h"

#ifdef __cplusplus
extern "C"
{
#endif

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

extern void 
jsonrpc_impl_proxy_init(
    struct jsonrpc_proxy * proxy,
    struct wf_impl_timeout_manager * manager,
    int timeout,
    jsonrpc_send_fn * send,
    void * user_data);

extern void 
jsonrpc_impl_proxy_cleanup(
    struct jsonrpc_proxy * proxy);

extern struct jsonrpc_proxy *
jsonrpc_impl_proxy_create(
    struct wf_impl_timeout_manager * manager,
    int timeout,
    jsonrpc_send_fn * send,
    void * user_data);

extern void
jsonrpc_impl_proxy_dispose(
    struct jsonrpc_proxy * proxy);


extern void
jsonrpc_impl_proxy_invoke(
	struct jsonrpc_proxy * proxy,
	jsonrpc_proxy_finished_fn * finished,
	void * user_data,
	char const * method_name,
	char const * param_info,
	va_list args
);

extern void
jsonrpc_impl_proxy_notify(
	struct jsonrpc_proxy * proxy,
	char const * method_name,
	char const * param_info,
	va_list args
);

extern void
jsonrpc_impl_proxy_onresult(
    struct jsonrpc_proxy * proxy,
    json_t * message);


#ifdef __cplusplus
}
#endif

#endif
