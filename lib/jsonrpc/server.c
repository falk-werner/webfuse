#include "jsonrpc/server.h"
#include "jsonrpc/method.h"
#include "jsonrpc/request.h"
#include "webfuse/core/util.h"

#include <string.h>

void jsonrpc_server_init(
    struct jsonrpc_server * server)
{
    server->methods = NULL;
}

void jsonrpc_server_cleanup(
    struct jsonrpc_server * server)
{
    struct jsonrpc_method * current = server->methods;
    while (NULL != current)
    {
        struct jsonrpc_method * next = current->next;
        jsonrpc_method_dispose(current);
        current = next;
    }
    server->methods = NULL;
}

void jsonrpc_server_add(
    struct jsonrpc_server * server,
    char const * method_name,
    jsonrpc_method_invoke_fn * invoke,
    void * user_data)
{
    struct jsonrpc_method * method = jsonrpc_method_create(method_name, invoke, user_data);
    method->next = server->methods;
    server->methods = method;
}

static void jsonrpc_server_invalid_method_invoke(
    struct jsonrpc_request * request,
    char const * WF_UNUSED_PARAM(method_name),
    json_t * WF_UNUSED_PARAM(params),
    void * WF_UNUSED_PARAM(user_data))
{
    jsonrpc_respond_error(request, WF_BAD_NOTIMPLEMENTED);
}

static struct jsonrpc_method const jsonrpc_server_invalid_method = 
{
    .next = NULL,
    .name = "<invalid>",
    .invoke = &jsonrpc_server_invalid_method_invoke,
    .user_data = NULL    
};

static struct jsonrpc_method const * jsonrpc_server_get_method(
    struct jsonrpc_server * server,
    char const * method_name)
{
    struct jsonrpc_method const * current = server->methods;
    while (NULL != current) 
    {
        if (0 == strcmp(method_name, current->name))
        {
            return current;
        }

        current = current->next;
    }

    return &jsonrpc_server_invalid_method;
}

void jsonrpc_server_process(
    struct jsonrpc_server * server,
    json_t * request_data,
    jsonrpc_send_fn * send,
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
        struct jsonrpc_request * request = jsonrpc_request_create(id, send, user_data);
        struct jsonrpc_method const * method = jsonrpc_server_get_method(server, method_name);

        method->invoke(request, method_name, params, method->user_data);
    }
}

