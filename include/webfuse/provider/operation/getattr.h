#ifndef WFP_OPERATION_GETATTR_H
#define WFP_OPERATION_GETATTR_H

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "webfuse/provider/api.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wfp_request;

typedef void wfp_getattr_fn(
    struct wfp_request * request,
    ino_t inode,
    void * user_data);

extern WFP_API void wfp_respond_getattr(
    struct wfp_request * request,
    struct stat const * stat);


#ifdef __cplusplus
}
#endif

#endif
