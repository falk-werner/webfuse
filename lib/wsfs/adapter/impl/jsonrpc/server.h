#ifndef WSFS_ADAPTER_JSONRPC_SERVER_H
#define WSFS_ADAPTER_JSONRPC_SERVER_H

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


struct wsfs_jsonrpc_request
{
    bool is_pending;
    wsfs_jsonrpc_method_finished_fn * finished;
    void * user_data;
    int id;
    struct wsfs_timer timer;
};

struct wsfs_jsonrpc_server
{
    struct wsfs_jsonrpc_method * methods;
    struct wsfs_jsonrpc_request request;
};


#ifdef __cplusplus
extern "C"
{
#endif

extern void wsfs_jsonrpc_server_init(
    struct wsfs_jsonrpc_server * server,
    struct wsfs_timeout_manager * manager);

extern void wsfs_jsonrpc_server_cleanup(
    struct wsfs_jsonrpc_server * server);

extern void wsfs_jsonrpc_server_add(
    struct wsfs_jsonrpc_server * server,
    char const * name,
    wsfs_jsonrpc_method_invoke_fn * invoke,
    void * user_data );

extern void wsfs_jsonrpc_server_invoke(
	struct wsfs_jsonrpc_server * server,
	wsfs_jsonrpc_method_finished_fn * finished,
	void * user_data,
	char const * method_name,
	char const * param_info,
	...
);

extern void wsfs_jsonrpc_server_notify(
	struct wsfs_jsonrpc_server * server,
	char const * method_name,
	char const * param_info,
	...
);

extern void wsfs_jsonrpc_server_onresult(
    struct wsfs_jsonrpc_server * server,
    char const * message,
    size_t length);

#ifdef __cplusplus
}
#endif


#endif
