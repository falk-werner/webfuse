#include "wf/jsonrpc.h"

#include "wf/jsonrpc/impl/proxy.h"
#include "wf/jsonrpc/impl/request.h"
#include "wf/jsonrpc/impl/response.h"
#include "wf/jsonrpc/impl/server.h"

// proxy

struct wf_jsonrpc_proxy *
wf_jsonrpc_proxy_create(
    struct wf_timer_manager * manager,
    int timeout,
    wf_jsonrpc_send_fn * send,
    void * user_data)
{
    return wf_jsonrpc_impl_proxy_create(manager, timeout, send, user_data);
}

void wf_jsonrpc_proxy_dispose(
    struct wf_jsonrpc_proxy * proxy)
{
    wf_jsonrpc_impl_proxy_dispose(proxy);
}

void wf_jsonrpc_proxy_invoke(
	struct wf_jsonrpc_proxy * proxy,
	wf_jsonrpc_proxy_finished_fn * finished,
	void * user_data,
	char const * method_name,
	char const * param_info,
	...
)
{
    va_list args;
    va_start(args, param_info);
    wf_jsonrpc_impl_proxy_invoke(proxy, finished, user_data, method_name, param_info, args);
    va_end(args);
}

void wf_jsonrpc_proxy_notify(
	struct wf_jsonrpc_proxy * proxy,
	char const * method_name,
	char const * param_info,
	...
)
{
    va_list args;
    va_start(args, param_info);
    wf_jsonrpc_impl_proxy_notify(proxy, method_name, param_info, args);
    va_end(args);
}

void wf_jsonrpc_proxy_onresult(
    struct wf_jsonrpc_proxy * proxy,
    json_t * message)
{
    wf_jsonrpc_impl_proxy_onresult(proxy, message);    
}


// request

bool wf_jsonrpc_is_request(
    json_t * message)
{
    return wf_jsonrpc_impl_is_request(message);
}

struct wf_jsonrpc_request * wf_jsonrpc_request_create(
    int id,
    wf_jsonrpc_send_fn * send,
    void * user_data)
{
    return wf_jsonrpc_impl_request_create(id, send, user_data);
}

void wf_jsonrpc_request_dispose(
    struct wf_jsonrpc_request * request)
{
    wf_jsonrpc_impl_request_dispose(request);
}

void * wf_jsonrpc_request_get_userdata(
    struct wf_jsonrpc_request * request)
{
    return wf_jsonrpc_impl_request_get_userdata(request);
}

void wf_jsonrpc_respond(
    struct wf_jsonrpc_request * request,
    json_t * result)
{
    wf_jsonrpc_impl_respond(request, result);
}

void wf_jsonrpc_respond_error(
    struct wf_jsonrpc_request * request,
    int code,
    char const * message)
{
    wf_jsonrpc_impl_respond_error(request, code, message);
}

// response

bool wf_jsonrpc_is_response(
    json_t * message)
{
    return wf_jsonrpc_impl_is_response(message);
}

// server

struct wf_jsonrpc_server *
wf_jsonrpc_server_create(void)
{
    return wf_jsonrpc_impl_server_create();
}

void
wf_jsonrpc_server_dispose(
    struct wf_jsonrpc_server * server)
{
    wf_jsonrpc_impl_server_dispose(server);
}

void wf_jsonrpc_server_add(
    struct wf_jsonrpc_server * server,
    char const * method_name,
    wf_jsonrpc_method_invoke_fn * invoke,
    void * user_data)
{
    wf_jsonrpc_impl_server_add(server, method_name, invoke, user_data);
}

void wf_jsonrpc_server_process(
    struct wf_jsonrpc_server * server,
    json_t * request,
    wf_jsonrpc_send_fn * send,
    void * user_data)
{
    wf_jsonrpc_impl_server_process(server, request, send, user_data);
}
