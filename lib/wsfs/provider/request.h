#ifndef WSFSP_REQUEST_H
#define WSFSP_REQUEST_H

#include <jansson.h>
#include "wsfs/provider/provider.h"

typedef void wsfsp_request_respond_fn(
    json_t * response,
    void * user_data);

struct wsfsp_request
{
    wsfsp_request_respond_fn * respond;
    void * user_data;
    int id;
};


#ifdef __cplusplus
extern "C"
{
#endif

extern struct wsfsp_request * wsfsp_request_create(
    struct wsfsp_request * prototype,
    int id);
            
extern void wsfsp_request_dispose(
    struct wsfsp_request * request);

#ifdef __cplusplus    
}
#endif

#endif
