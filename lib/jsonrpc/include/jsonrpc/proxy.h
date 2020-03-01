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
#include <jsonrpc/api.h>
#include <jsonrpc/send_fn.h>
#include <jsonrpc/proxy_finished_fn.h>

#ifdef __cplusplus
extern "C" {
#endif

struct jsonrpc_proxy;
struct wf_timer_manager;

extern JSONRPC_API struct jsonrpc_proxy *
jsonrpc_proxy_create(
    struct wf_timer_manager * manager,
    int timeout,
    jsonrpc_send_fn * send,
    void * user_data);

extern JSONRPC_API void jsonrpc_proxy_dispose(
    struct jsonrpc_proxy * proxy);

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
extern JSONRPC_API void jsonrpc_proxy_invoke(
	struct jsonrpc_proxy * proxy,
	jsonrpc_proxy_finished_fn * finished,
	void * user_data,
	char const * method_name,
	char const * param_info,
	...
);

extern JSONRPC_API void jsonrpc_proxy_notify(
	struct jsonrpc_proxy * proxy,
	char const * method_name,
	char const * param_info,
	...
);

extern JSONRPC_API void jsonrpc_proxy_onresult(
    struct jsonrpc_proxy * proxy,
    json_t * message);

#ifdef __cplusplus
}
#endif


#endif
