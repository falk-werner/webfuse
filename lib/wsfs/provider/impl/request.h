#ifndef WSFS_PROVIDER_IMPL_REQUEST_H
#define WSFS_PROVIDER_IMPL_REQUEST_H

#include <jansson.h>
#include "wsfs/provider/impl/provider.h"
#include "wsfs/core/status.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef void wsfsp_impl_request_respond_fn(
    json_t * response,
    void * user_data);

struct wsfsp_request
{
    wsfsp_impl_request_respond_fn * respond;
    void * user_data;
    int id;
};

extern void wsfsp_impl_respond_error(
    struct wsfsp_request * request,
    wsfs_status status);

extern struct wsfsp_request * wsfsp_impl_request_create(
    struct wsfsp_request * prototype,
    int id);
            
extern void wsfsp_impl_request_dispose(
    struct wsfsp_request * request);

extern void wsfsp_impl_respond(
    struct wsfsp_request * request,
    json_t * result);

#ifdef __cplusplus    
}
#endif

#endif
