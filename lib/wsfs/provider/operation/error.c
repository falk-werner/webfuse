#include "wsfs/provider/operation/error.h"
#include <jansson.h>
#include "wsfs/provider/request.h"

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
