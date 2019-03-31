#include "webfuse/adapter/impl/jsonrpc/request.h"
#include "webfuse/core/status_intern.h"
#include <stdlib.h>

struct wf_impl_jsonrpc_request
{
    int id;
    wf_impl_jsonrpc_send_fn * send;
    void * user_data;
};

bool wf_impl_jsonrpc_is_request(
    json_t * message)
{
    json_t * id = json_object_get(message, "id");
    json_t * method = json_object_get(message, "method");
    json_t * params = json_object_get(message, "params");

    return (json_is_integer(id) && json_is_string(method) &&
            (json_is_array(params) || json_is_object(params)));
}


struct wf_impl_jsonrpc_request * wf_impl_jsonrpc_request_create(
    int id,
    wf_impl_jsonrpc_send_fn * send,
    void * user_data)
{
    struct wf_impl_jsonrpc_request * request = malloc(sizeof(struct wf_impl_jsonrpc_request));
    if (NULL != request)
    {
        request->id = id;
        request->send = send;
        request->user_data = user_data;
    }

    return request;
}

void wf_impl_jsonrpc_request_dispose(
    struct wf_impl_jsonrpc_request * request)
{
    free(request);
}


void wf_impl_jsonrpc_respond(
    struct wf_impl_jsonrpc_request * request,
    json_t * result)
{
    json_t * response = json_object();
    json_object_set_new(response, "result", result);
    json_object_set_new(response, "id", json_integer(request->id));

    request->send(response, request->user_data);
    json_decref(response);
    wf_impl_jsonrpc_request_dispose(request);
}

void wf_impl_jsonrpc_respond_error(
    struct wf_impl_jsonrpc_request * request,
    wf_status status)
{
    json_t * err = json_object();
    json_object_set_new(err, "code", json_integer(status));
    json_object_set_new(err, "message", json_string(wf_status_tostring(status)));

    json_t * response = json_object();
    json_object_set_new(response, "error", err);
    json_object_set_new(response, "id", json_integer(request->id));

    request->send(response, request->user_data);
    json_decref(response);
    wf_impl_jsonrpc_request_dispose(request);
}

