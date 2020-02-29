#include "jsonrpc.h"

#include "jsonrpc/impl/proxy.h"
#include "jsonrpc/impl/request.h"
#include "jsonrpc/impl/response.h"
#include "jsonrpc/impl/server.h"

// proxy

struct jsonrpc_proxy *
jsonrpc_proxy_create(
    struct wf_timer_manager * manager,
    int timeout,
    jsonrpc_send_fn * send,
    void * user_data)
{
    return jsonrpc_impl_proxy_create(manager, timeout, send, user_data);
}

void jsonrpc_proxy_dispose(
    struct jsonrpc_proxy * proxy)
{
    jsonrpc_impl_proxy_dispose(proxy);
}

void jsonrpc_proxy_invoke(
	struct jsonrpc_proxy * proxy,
	jsonrpc_proxy_finished_fn * finished,
	void * user_data,
	char const * method_name,
	char const * param_info,
	...
)
{
    va_list args;
    va_start(args, param_info);
    jsonrpc_impl_proxy_invoke(proxy, finished, user_data, method_name, param_info, args);
    va_end(args);
}

void jsonrpc_proxy_notify(
	struct jsonrpc_proxy * proxy,
	char const * method_name,
	char const * param_info,
	...
)
{
    va_list args;
    va_start(args, param_info);
    jsonrpc_impl_proxy_notify(proxy, method_name, param_info, args);
    va_end(args);
}

void jsonrpc_proxy_onresult(
    struct jsonrpc_proxy * proxy,
    json_t * message)
{
    jsonrpc_impl_proxy_onresult(proxy, message);    
}


// request

bool jsonrpc_is_request(
    json_t * message)
{
    return jsonrpc_impl_is_request(message);
}

struct jsonrpc_request * jsonrpc_request_create(
    int id,
    jsonrpc_send_fn * send,
    void * user_data)
{
    return jsonrpc_impl_request_create(id, send, user_data);
}

void jsonrpc_request_dispose(
    struct jsonrpc_request * request)
{
    jsonrpc_impl_request_dispose(request);
}

void * jsonrpc_request_get_userdata(
    struct jsonrpc_request * request)
{
    return jsonrpc_impl_request_get_userdata(request);
}

void jsonrpc_respond(
    struct jsonrpc_request * request,
    json_t * result)
{
    jsonrpc_impl_respond(request, result);
}

void jsonrpc_respond_error(
    struct jsonrpc_request * request,
    int code,
    char const * message)
{
    jsonrpc_impl_respond_error(request, code, message);
}

// response

bool jsonrpc_is_response(
    json_t * message)
{
    return jsonrpc_impl_is_response(message);
}

// server

struct jsonrpc_server *
jsonrpc_server_create(void)
{
    return jsonrpc_impl_server_create();
}

void
jsonrpc_server_dispose(
    struct jsonrpc_server * server)
{
    jsonrpc_impl_server_dispose(server);
}

void jsonrpc_server_add(
    struct jsonrpc_server * server,
    char const * method_name,
    jsonrpc_method_invoke_fn * invoke,
    void * user_data)
{
    jsonrpc_impl_server_add(server, method_name, invoke, user_data);
}

void jsonrpc_server_process(
    struct jsonrpc_server * server,
    json_t * request,
    jsonrpc_send_fn * send,
    void * user_data)
{
    jsonrpc_impl_server_process(server, request, send, user_data);
}
