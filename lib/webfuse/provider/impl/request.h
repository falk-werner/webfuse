#ifndef WF_PROVIDER_IMPL_REQUEST_H
#define WF_PROVIDER_IMPL_REQUEST_H

#include <jansson.h>
#include "webfuse/provider/impl/provider.h"
#include "webfuse/core/status.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef void wfp_impl_request_respond_fn(
    json_t * response,
    void * user_data);

struct wfp_request
{
    wfp_impl_request_respond_fn * respond;
    void * user_data;
    int id;
};

extern void wfp_impl_respond_error(
    struct wfp_request * request,
    wf_status status);

extern struct wfp_request * wfp_impl_request_create(
    struct wfp_request * prototype,
    int id);
            
extern void wfp_impl_request_dispose(
    struct wfp_request * request);

extern void wfp_impl_respond(
    struct wfp_request * request,
    json_t * result);

#ifdef __cplusplus    
}
#endif

#endif
