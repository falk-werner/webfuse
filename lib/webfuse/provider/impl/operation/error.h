#ifndef WFP_OPERATION_IMPL_ERROR_H
#define WFP_OPERATION_IMPL_ERROR_H

#include "webfuse/provider/api.h"
#include "webfuse/core/status.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wfp_request;

extern WFP_API void wfp_impl_respond_error(
    struct wfp_request * request,
    wf_status status);

#ifdef __cplusplus
}
#endif

#endif
