#include "webfuse/core/jsonrpc/error.h"

json_t *
wf_jsonrpc_error(
    int code,
    char const * message)
{
    json_t * error = json_object();
    json_object_set_new(error, "code", json_integer(code));
    json_object_set_new(error, "message", json_string(message));

    return error;
}

void
wf_jsonrpc_propate_error(
    wf_jsonrpc_proxy_finished_fn * finised,
    void * user_data,
    int code,
    char const * message)
{
    json_t * error = wf_jsonrpc_error(code, message);
    finised(user_data, NULL, error);

    json_decref(error);
}

