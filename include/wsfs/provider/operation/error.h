#ifndef WSFSP_OPERATION_ERROR_H
#define WSFSP_OPERATION_ERROR_H

#include "wsfs/provider/api.h"
#include "wsfs/core/status.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wsfsp_request;

extern WSFSP_API void wsfsp_respond_error(
    struct wsfsp_request * request,
    wsfs_status status);

#ifdef __cplusplus
}
#endif

#endif
