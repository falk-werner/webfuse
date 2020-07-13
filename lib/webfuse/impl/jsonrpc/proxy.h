#ifndef WF_IMPL_JSONRPC_PROXY_H
#define WF_IMPL_JSONRPC_PROXY_H

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
#include "webfuse/impl/jsonrpc/send_fn.h"
#include "webfuse/impl/jsonrpc/proxy_finished_fn.h"

#ifdef __cplusplus
extern "C" {
#endif

struct wf_jsonrpc_proxy;
struct wf_timer_manager;
struct wf_json_writer;

typedef void
wf_jsonrpc_custom_write_fn(
	struct wf_json_writer * writer,
	void * data);

extern struct wf_jsonrpc_proxy *
wf_impl_jsonrpc_proxy_create(
    struct wf_timer_manager * manager,
    int timeout,
    wf_jsonrpc_send_fn * send,
    void * user_data);

extern void wf_impl_jsonrpc_proxy_dispose(
    struct wf_jsonrpc_proxy * proxy);

//------------------------------------------------------------------------------
/// \brief Invokes a method.
///
/// Creates a method an sends it using the send function.
/// Proxy keeps track of method invokation. If no response is returned within
/// timeout, an error is propagated.
///
/// \param proxy pointer to proxy instance
/// \param finished function which is called exactly once, either on success or
///                 on failure.
/// \param method_name name of the method to invoke
/// \param param_info types of the param (s = string, i = integer, j = json)
/// \param ... params
//------------------------------------------------------------------------------
extern void wf_impl_jsonrpc_proxy_invoke(
	struct wf_jsonrpc_proxy * proxy,
	wf_jsonrpc_proxy_finished_fn * finished,
	void * user_data,
	char const * method_name,
	char const * param_info,
	...
);

extern void wf_impl_jsonrpc_proxy_notify(
	struct wf_jsonrpc_proxy * proxy,
	char const * method_name,
	char const * param_info,
	...
);

extern void wf_impl_jsonrpc_proxy_onresult(
    struct wf_jsonrpc_proxy * proxy,
    json_t * message);

#ifdef __cplusplus
}
#endif


#endif
