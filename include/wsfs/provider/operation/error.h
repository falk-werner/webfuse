#ifndef WSFSP_OPERATION_ERROR_H
#define WSFSP_OPERATION_ERROR_H

#include "wsfs/provider/api.h"

struct wsfsp_request;

#ifdef __cplusplus
extern "C"
{
#endif

extern WSFSP_API void wsfsp_respond_error(
    struct wsfsp_request * request,
    int status);

#ifdef __cplusplus
}
#endif

#endif
