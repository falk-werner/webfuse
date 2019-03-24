#ifndef WSFS_ADAPTER_IMPL_JSONRPC_SERVER_H
#define WSFS_ADAPTER_IMPL_JSONRPC_SERVER_H

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
#include "wsfs/adapter/impl/jsonrpc/method.h"
#include "wsfs/adapter/impl/time/timeout_manager.h"
#include "wsfs/adapter/impl/time/timer.h"

#ifdef __cplusplus
extern "C" {
#endif

struct jsonrpc_request
{
    bool is_pending;
    jsonrpc_method_finished_fn * finished;
    void * user_data;
    int id;
    struct timer timer;
};

struct jsonrpc_server
{
    struct jsonrpc_method * methods;
    struct jsonrpc_request request;
};

extern void jsonrpc_server_init(
    struct jsonrpc_server * server,
    struct timeout_manager * manager);

extern void jsonrpc_server_cleanup(
    struct jsonrpc_server * server);

extern void jsonrpc_server_add(
    struct jsonrpc_server * server,
    char const * name,
    jsonrpc_method_invoke_fn * invoke,
    void * user_data );

extern void jsonrpc_server_invoke(
	struct jsonrpc_server * server,
	jsonrpc_method_finished_fn * finished,
	void * user_data,
	char const * method_name,
	char const * param_info,
	...
);

extern void jsonrpc_server_notify(
	struct jsonrpc_server * server,
	char const * method_name,
	char const * param_info,
	...
);

extern void jsonrpc_server_onresult(
    struct jsonrpc_server * server,
    char const * message,
    size_t length);

#ifdef __cplusplus
}
#endif


#endif
