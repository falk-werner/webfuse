#include "webfuse/impl/jsonrpc/request.h"
#include "webfuse/impl/jsonrpc/error.h"
#include "webfuse/impl/json/writer.h"
#include "webfuse/impl/json/node.h"
#include "webfuse/impl/jsonrpc/response_writer.h"
#include "webfuse/impl/message.h"

#include <libwebsockets.h>
#include <stdlib.h>

struct wf_jsonrpc_request
{
    struct wf_jsonrpc_response_writer * writer;
    int id;
    wf_jsonrpc_send_fn * send;
    void * user_data;
};

bool
wf_impl_jsonrpc_is_request(
    struct wf_json const * message)
{
    if (NULL == message) { return false; }

    struct wf_json const * id = wf_impl_json_object_get(message, "id");
    struct wf_json const * method = wf_impl_json_object_get(message, "method");
    struct wf_json const * params = wf_impl_json_object_get(message, "params");

    return ( (wf_impl_json_is_int(id)) && (wf_impl_json_is_string(method)) &&
            ( (wf_impl_json_is_array(params)) || (wf_impl_json_is_object(params)) ));
}


struct wf_jsonrpc_request *
wf_impl_jsonrpc_request_create(
    int id,
    wf_jsonrpc_send_fn * send,
    void * user_data)
{
    struct wf_jsonrpc_request * request = malloc(sizeof(struct wf_jsonrpc_request));
    request->writer = wf_impl_jsonrpc_response_writer_create(id);
    request->id = id;
    request->send = send;
    request->user_data = user_data;

    return request;
}

void
wf_impl_jsonrpc_request_dispose(
    struct wf_jsonrpc_request * request)
{
    wf_impl_jsonrpc_response_writer_dispose(request->writer);
    free(request);
}

void *
wf_impl_jsonrpc_request_get_userdata(
    struct wf_jsonrpc_request * request)
{
    return request->user_data;
}

struct wf_jsonrpc_response_writer *
wf_impl_jsonrpc_request_get_response_writer(
    struct wf_jsonrpc_request * request)
{
    return request->writer;
}


void
wf_impl_jsonrpc_respond(
    struct wf_jsonrpc_request * request)
{
    struct wf_message * response = wf_impl_jsonrpc_response_writer_take_message(request->writer);

    request->send(response, request->user_data);
    wf_impl_jsonrpc_request_dispose(request);
}

void wf_impl_jsonrpc_respond_error(
    struct wf_jsonrpc_request * request,
    int code,
    char const * message)
{
    struct wf_json_writer * writer = wf_impl_json_writer_create(128, LWS_PRE);
    wf_impl_json_write_object_begin(writer);
    wf_impl_json_write_object_begin_object(writer, "error");
    wf_impl_json_write_object_int(writer, "code", code);
    wf_impl_json_write_object_string(writer, "message", message);
    wf_impl_json_write_object_end(writer);
    wf_impl_json_write_object_int(writer, "id", request->id);
    wf_impl_json_write_object_end(writer);

    size_t length;
    char * data = wf_impl_json_writer_take(writer, &length);
    wf_impl_json_writer_dispose(writer);

    struct wf_message * response = wf_impl_message_create(data, length);
    request->send(response, request->user_data);

    wf_impl_jsonrpc_request_dispose(request);
}
