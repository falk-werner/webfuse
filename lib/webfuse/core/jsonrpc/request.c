#include "webfuse/core/jsonrpc/request.h"
#include "webfuse/core/jsonrpc/error.h"
#include <stdlib.h>

struct wf_jsonrpc_request
{
    int id;
    wf_jsonrpc_send_fn * send;
    void * user_data;
};

bool
wf_jsonrpc_is_request(
    json_t * message)
{
    json_t * id = json_object_get(message, "id");
    json_t * method = json_object_get(message, "method");
    json_t * params = json_object_get(message, "params");

    return (json_is_integer(id) && json_is_string(method) &&
            (json_is_array(params) || json_is_object(params)));
}


struct wf_jsonrpc_request *
wf_jsonrpc_request_create(
    int id,
    wf_jsonrpc_send_fn * send,
    void * user_data)
{
    struct wf_jsonrpc_request * request = malloc(sizeof(struct wf_jsonrpc_request));
    request->id = id;
    request->send = send;
    request->user_data = user_data;

    return request;
}

void
wf_jsonrpc_request_dispose(
    struct wf_jsonrpc_request * request)
{
    free(request);
}

void *
wf_jsonrpc_request_get_userdata(
    struct wf_jsonrpc_request * request)
{
    return request->user_data;
}


void
wf_jsonrpc_respond(
    struct wf_jsonrpc_request * request,
    json_t * result)
{
    json_t * response = json_object();
    json_object_set_new(response, "result", result);
    json_object_set_new(response, "id", json_integer(request->id));

    request->send(response, request->user_data);
    json_decref(response);
    wf_jsonrpc_request_dispose(request);
}

void wf_jsonrpc_respond_error(
    struct wf_jsonrpc_request * request,
    int code,
    char const * message)
{
    json_t * response = json_object();
    json_object_set_new(response, "error", wf_jsonrpc_error(code, message));
    json_object_set_new(response, "id", json_integer(request->id));

    request->send(response, request->user_data);
    json_decref(response);
    wf_jsonrpc_request_dispose(request);
}

