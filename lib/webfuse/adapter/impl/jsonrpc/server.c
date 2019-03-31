#include "webfuse/adapter/impl/jsonrpc/server.h"
#include "webfuse/adapter/impl/jsonrpc/method.h"
#include "webfuse/adapter/impl/jsonrpc/request.h"
#include "webfuse/core/util.h"

#include <string.h>

void wf_impl_jsonrpc_server_init(
    struct wf_impl_jsonrpc_server * server)
{
    server->methods = NULL;
}

void wf_impl_jsonrpc_server_cleanup(
    struct wf_impl_jsonrpc_server * server)
{
    struct wf_impl_jsonrpc_method * current = server->methods;
    while (NULL != current)
    {
        struct wf_impl_jsonrpc_method * next = current->next;
        wf_impl_jsonrpc_method_dispose(current);
        current = next;
    }
    server->methods = NULL;
}

void wf_impl_jsonrpc_server_add(
    struct wf_impl_jsonrpc_server * server,
    char const * method_name,
    wf_impl_jsonrpc_method_invoke_fn * invoke,
    void * user_data)
{
    struct wf_impl_jsonrpc_method * method = wf_impl_jsonrpc_method_create(method_name, invoke, user_data);
    method->next = server->methods;
    server->methods = method;
}

static void wf_impl_jsonrpc_server_invalid_method_invoke(
    struct wf_impl_jsonrpc_request * request,
    char const * WF_UNUSED_PARAM(method_name),
    json_t * WF_UNUSED_PARAM(params),
    void * WF_UNUSED_PARAM(user_data))
{
    wf_impl_jsonrpc_respond_error(request, WF_BAD_NOTIMPLEMENTED);
}

static struct wf_impl_jsonrpc_method const wf_impl_jsonrpc_server_invalid_method = 
{
    .next = NULL,
    .name = "<invalid>",
    .invoke = &wf_impl_jsonrpc_server_invalid_method_invoke,
    .user_data = NULL    
};

static struct wf_impl_jsonrpc_method const * wf_impl_jsonrpc_server_get_method(
    struct wf_impl_jsonrpc_server * server,
    char const * method_name)
{
    struct wf_impl_jsonrpc_method const * current = server->methods;
    while (NULL != current) 
    {
        if (0 == strcmp(method_name, current->name))
        {
            return current;
        }

        current = current->next;
    }

    return &wf_impl_jsonrpc_server_invalid_method;
}

void wf_impl_jsonrpc_server_process(
    struct wf_impl_jsonrpc_server * server,
    json_t * request_data,
    wf_impl_jsonrpc_send_fn * send,
    void * user_data)
{
    json_t * method_holder = json_object_get(request_data, "method");
    json_t * params = json_object_get(request_data, "params");
    json_t * id_holder = json_object_get(request_data, "id");

    if (json_is_string(method_holder) &&
        (json_is_array(params) || (json_is_object(params))) &&
        json_is_integer(id_holder))
    {
        char const * method_name = json_string_value(method_holder);
        int id = json_integer_value(id_holder);
        struct wf_impl_jsonrpc_request * request = wf_impl_jsonrpc_request_create(id, send, user_data);
        struct wf_impl_jsonrpc_method const * method = wf_impl_jsonrpc_server_get_method(server, method_name);

        method->invoke(request, method_name, params, method->user_data);
    }
}

