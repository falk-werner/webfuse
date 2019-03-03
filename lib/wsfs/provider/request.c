#include "wsfs/provider/request.h"

#include <stdlib.h>
#include "wsfs/provider/operation/error.h"

struct wsfsp_request * wsfsp_request_create(
    struct wsfsp_request * prototype,
    int id)
{
    struct wsfsp_request * request = malloc(sizeof(struct wsfsp_request));
    if (NULL != request)
    {
        request->respond = prototype->respond;
        request->user_data = prototype->user_data;
        request->id = id;
    }

    return request;
}

void wsfsp_request_dispose(
    struct wsfsp_request * request)
{
    free(request);
}

extern void wsfsp_respond(
    struct wsfsp_request * request,
    json_t * result)
{
    json_t * response = json_object();
    json_object_set_new(response, "result", result);
    json_object_set_new(response, "id", json_integer(request->id));

    request->respond(response, request->user_data);

    json_decref(response);
    wsfsp_request_dispose(request);
}

void wsfsp_respond_error(
    struct wsfsp_request * request,
    int status)
{
    json_t * response = json_object();
    json_t * error = json_object();
    json_object_set_new(error, "code", json_integer(status));
    json_object_set_new(response, "error", error);
    json_object_set_new(response, "id", json_integer(request->id));

    request->respond(response, request->user_data);

    json_decref(response);
    wsfsp_request_dispose(request);
}