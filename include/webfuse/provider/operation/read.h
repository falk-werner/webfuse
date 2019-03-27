#ifndef WFP_OPERATION_READ_H
#define WFP_OPERATION_READ_H

#ifndef __cplusplus
#include <stddef.h>
#include <inttypes.h>
#else
#include <cstddef>
#include <cinttypes>
using std::size_t;
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

typedef void wfp_read_fn(
    struct wfp_request * request,
    ino_t inode,
    uint32_t handle,
    size_t offset,
    size_t length,
    void * user_data);

extern WFP_API void wfp_respond_read(
    struct wfp_request * request,
    char const * data,
    size_t length);


#ifdef __cplusplus
}
#endif

#endif
