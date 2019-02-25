#include "wsfs/provider/request.h"

#include <stdlib.h>

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