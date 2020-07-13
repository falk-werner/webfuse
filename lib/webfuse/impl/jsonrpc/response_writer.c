#include "webfuse/impl/jsonrpc/response_writer.h"
#include "webfuse/impl/message.h"
#include "webfuse/impl/json/writer.h"

#include <libwebsockets.h>
#include <stdlib.h>

#define WF_RESPONSE_WRITER_DEFAULT_MESSAGE_SIZE 1024

struct wf_jsonrpc_response_writer
{
    struct wf_json_writer * json_writer;
    int id;
};

struct wf_jsonrpc_response_writer *
wf_impl_jsonrpc_response_writer_create(int id)
{
    struct wf_jsonrpc_response_writer * writer = malloc(sizeof(struct wf_jsonrpc_response_writer));
    writer->json_writer = wf_impl_json_writer_create(WF_RESPONSE_WRITER_DEFAULT_MESSAGE_SIZE, LWS_PRE);
    writer->id = id;

    wf_impl_json_write_object_begin(writer->json_writer);
    wf_impl_json_write_object_begin_object(writer->json_writer, "result");

    return writer;
}

void
wf_impl_jsonrpc_response_writer_dispose(
    struct wf_jsonrpc_response_writer * writer)
{
    wf_impl_json_writer_dispose(writer->json_writer);
    free(writer);
}

struct wf_message *
wf_impl_jsonrpc_response_writer_take_message(
    struct wf_jsonrpc_response_writer * writer)
{
    wf_impl_json_write_object_end(writer->json_writer);
    wf_impl_json_write_object_int(writer->json_writer, "id", writer->id);
    wf_impl_json_write_object_end(writer->json_writer);

    size_t length;
    char * data = wf_impl_json_writer_take(writer->json_writer, &length);

    return wf_impl_message_create(data, length);
}

void
wf_impl_jsonrpc_response_add_int(
    struct wf_jsonrpc_response_writer * writer,
    char const * key,
    int value)
{
    wf_impl_json_write_object_int(writer->json_writer, key, value);
}

void
wf_impl_jsonrpc_response_add_string(
    struct wf_jsonrpc_response_writer * writer,
    char const * key,
    char const * value)
{
    wf_impl_json_write_object_string(writer->json_writer, key, value);
}
