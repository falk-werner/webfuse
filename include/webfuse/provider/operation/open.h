#ifndef WFP_OPERATION_OPEN_H
#define WFP_OPERATION_OPEN_H

#ifndef __cplusplus
#include <inttypes.h>
#else
#include <cinttypes>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "webfuse/provider/api.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wfp_request;

typedef void wfp_open_fn(
    struct wfp_request * request,
    ino_t inode,
    int flags,
    void * user_data);

extern WFP_API void wfp_respond_open(
    struct wfp_request * request,
    uint32_t handle);


#ifdef __cplusplus
}
#endif

#endif
