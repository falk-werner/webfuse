#ifndef WSFSP_OPERATION_ERROR_H
#define WSFSP_OPERATION_ERROR_H

#include "wsfs/provider/api.h"
#include "wsfs/core/status.h"

struct wsfsp_request;

#ifdef __cplusplus
extern "C"
{
#endif

extern WSFSP_API void wsfsp_respond_error(
    struct wsfsp_request * request,
    wsfs_status status);

#ifdef __cplusplus
}
#endif

#endif
