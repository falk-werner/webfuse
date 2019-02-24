#ifndef WSFSP_REQUEST_H
#define WSFSP_REQUEST_H

#include <jansson.h>
#include "wsfsp/provider.h"

typedef void wsfsp_request_respond_fn(
    json_t * response,
    void * user_data);

struct wsfsp_request
{
    wsfsp_request_respond_fn * respond;
    void * user_data;
};


#endif
