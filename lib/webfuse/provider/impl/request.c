#include "webfuse/provider/impl/request.h"

#include <stdlib.h>
#include "webfuse/provider/impl/operation/error.h"

struct wfp_request * wfp_impl_request_create(
    struct wfp_request * prototype,
    int id)
{
    struct wfp_request * request = malloc(sizeof(struct wfp_request));
    request->respond = prototype->respond;
    request->user_data = prototype->user_data;
    request->id = id;

    return request;
}

void wfp_impl_request_dispose(
    struct wfp_request * request)
{
    free(request);
}

extern void wfp_impl_respond(
    struct wfp_request * request,
    json_t * result)
{
    json_t * response = json_object();
    json_object_set_new(response, "result", result);
    json_object_set_new(response, "id", json_integer(request->id));

    request->respond(response, request->user_data);

    json_decref(response);
    wfp_impl_request_dispose(request);
}

void wfp_impl_respond_error(
    struct wfp_request * request,
    wf_status status)
{
    json_t * response = json_object();
    json_t * error = json_object();
    json_object_set_new(error, "code", json_integer(status));
    json_object_set_new(response, "error", error);
    json_object_set_new(response, "id", json_integer(request->id));

    request->respond(response, request->user_data);

    json_decref(response);
    wfp_impl_request_dispose(request);
}