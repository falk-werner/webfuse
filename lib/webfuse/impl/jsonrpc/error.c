#include "webfuse/impl/jsonrpc/error.h"

#include <stdlib.h>
#include <string.h>

struct wf_jsonrpc_error
{
    int code;
    char * message;
};

struct wf_jsonrpc_error *
wf_impl_jsonrpc_error(
    int code,
    char const * message)
{
    struct wf_jsonrpc_error * error = malloc(sizeof(struct wf_jsonrpc_error));
    error->code = code;
    error->message = strdup(message);

    return error;
}

void
wf_impl_jsonrpc_error_dispose(
    struct wf_jsonrpc_error * error)
{
    if (NULL != error)
    {
        free(error->message);
        free(error);
    }
}

int
wf_impl_jsonrpc_error_code(
    struct wf_jsonrpc_error const * error)
{
    return error->code;
}

char const *
wf_impl_jsonrpc_error_message(
    struct wf_jsonrpc_error const * error)
{
    return error->message;
}

void
wf_impl_jsonrpc_propate_error(
    wf_jsonrpc_proxy_finished_fn * finised,
    void * user_data,
    int code,
    char const * message)
{
    struct wf_jsonrpc_error error =
    {
        .code = code,
        .message = (char*) message        
    };

    finised(user_data, NULL, &error);
}

