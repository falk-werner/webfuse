#include "webfuse/impl/jsonrpc/server.h"
#include "webfuse/impl/jsonrpc/method.h"
#include "webfuse/impl/jsonrpc/request.h"
#include "webfuse/impl/json/node.h"
#include "webfuse/status.h"
#include "webfuse/impl/util/util.h"

#include <stdlib.h>
#include <string.h>

struct wf_jsonrpc_server
{
    struct wf_jsonrpc_method * methods;
};

static void
wf_impl_jsonrpc_server_init(
    struct wf_jsonrpc_server * server);

static void
wf_impl_jsonrpc_server_cleanup(
    struct wf_jsonrpc_server * server);

struct wf_jsonrpc_server *
wf_impl_jsonrpc_server_create(void)
{
    struct wf_jsonrpc_server * server = malloc(sizeof(struct wf_jsonrpc_server));
    wf_impl_jsonrpc_server_init(server);

    return server;
}

void
wf_impl_jsonrpc_server_dispose(
    struct wf_jsonrpc_server * server)
{
    wf_impl_jsonrpc_server_cleanup(server);
    free(server);
}

static void wf_impl_jsonrpc_server_init(
    struct wf_jsonrpc_server * server)
{
    server->methods = NULL;
}

static void wf_impl_jsonrpc_server_cleanup(
    struct wf_jsonrpc_server * server)
{
    struct wf_jsonrpc_method * current = server->methods;
    while (NULL != current)
    {
        struct wf_jsonrpc_method * next = current->next;
        wf_impl_jsonrpc_method_dispose(current);
        current = next;
    }
    server->methods = NULL;
}

void wf_impl_jsonrpc_server_add(
    struct wf_jsonrpc_server * server,
    char const * method_name,
    wf_jsonrpc_method_invoke_fn * invoke,
    void * user_data)
{
    struct wf_jsonrpc_method * method = wf_impl_jsonrpc_method_create(method_name, invoke, user_data);
    method->next = server->methods;
    server->methods = method;
}

static void wf_impl_jsonrpc_server_invalid_method_invoke(
    struct wf_jsonrpc_request * request,
    char const * WF_UNUSED_PARAM(method_name),
    struct wf_json const * WF_UNUSED_PARAM(params),
    void * WF_UNUSED_PARAM(user_data))
{
    wf_impl_jsonrpc_respond_error(request, WF_BAD_NOTIMPLEMENTED, "not implemented");
}

static struct wf_jsonrpc_method const wf_impl_jsonrpc_server_invalid_method = 
{
    .next = NULL,
    .name = "<invalid>",
    .invoke = &wf_impl_jsonrpc_server_invalid_method_invoke,
    .user_data = NULL    
};

static struct wf_jsonrpc_method const *
wf_impl_jsonrpc_server_get_method(
    struct wf_jsonrpc_server * server,
    char const * method_name)
{
    struct wf_jsonrpc_method const * current = server->methods;
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
    struct wf_jsonrpc_server * server,
    struct wf_json const * request_data,
    wf_jsonrpc_send_fn * send,
    void * user_data)
{
    struct wf_json const * method_holder = wf_impl_json_object_get(request_data, "method");
    struct wf_json const * params = wf_impl_json_object_get(request_data, "params");
    struct wf_json const * id_holder = wf_impl_json_object_get(request_data, "id");

    if ((WF_JSON_TYPE_STRING == wf_impl_json_type(method_holder)) &&
        ((WF_JSON_TYPE_ARRAY == wf_impl_json_type(params)) || (WF_JSON_TYPE_OBJECT == wf_impl_json_type(params))) &&
        (WF_JSON_TYPE_INT == wf_impl_json_type(id_holder)))
    {
        char const * method_name = wf_impl_json_string_get(method_holder);
        int id = wf_impl_json_int_get(id_holder);
        struct wf_jsonrpc_request * request = wf_impl_jsonrpc_request_create(id, send, user_data);
        struct wf_jsonrpc_method const * method = wf_impl_jsonrpc_server_get_method(server, method_name);

        method->invoke(request, method_name, params, method->user_data);
    }
}

