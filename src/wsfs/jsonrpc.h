#ifndef _WSFS_JSONRPC_H
#define _WSFS_JSONRPC_H

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
#include "wsfs/status.h"

struct wsfs_jsonrpc;

typedef char * wsfs_create_message_fn(size_t size);
typedef bool wsfs_send_message_fn(char * message, size_t length, void * user_data);

#ifdef __cplusplus
extern "C" {
#endif

extern struct wsfs_jsonrpc * wsfs_jsonrpc_create(
	wsfs_create_message_fn * create_message,
	wsfs_send_message_fn * send_message,
	void * user_data);

extern void wsfs_jsonrpc_set_user_data(
	struct wsfs_jsonrpc * rpc,
	void * user_data);

extern void wsfs_jsonrpc_dispose(
	struct wsfs_jsonrpc * rpc);

extern wsfs_status wsfs_jsonrpc_invoke(
	struct wsfs_jsonrpc * rpc,
	json_t * * result,
	char const * method,
	char const * param_info,
	...
);

extern void wsfs_jsonrpc_on_message(
	char const * message,
	size_t length,
	void * user_data);

#ifdef __cplusplus
}
#endif

#endif

