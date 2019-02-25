#ifndef WSFSP_OPERATION_OPEN_H
#define WSFSP_OPERATION_OPEN_H

#ifndef __cplusplus
#include <inttypes.h>
#else
#include <cinttypes>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "wsfs/provider/api.h"

struct wsfsp_request;

typedef void wsfsp_open_fn(
    struct wsfsp_request * request,
    ino_t inode,
    int flags,
    void * user_data);

#ifdef __cplusplus
extern "C"
{
#endif

extern WSFSP_API void wsfsp_respond_open(
    struct wsfsp_request * request,
    uint32_t handle);


#ifdef __cplusplus
}
#endif

#endif
