#ifndef WSFSP_OPERATION_READ_H
#define WSFSP_OPERATION_READ_H

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

#include "wsfs/provider/api.h"

struct wsfsp_request;

typedef void wsfsp_read_fn(
    struct wsfsp_request * request,
    ino_t inode,
    uint32_t handle,
    size_t offset,
    size_t length,
    void * user_data);

#ifdef __cplusplus
extern "C"
{
#endif

extern WSFSP_API void wsfsp_respond_read(
    struct wsfsp_request * request,
    char const * data,
    size_t length);


#ifdef __cplusplus
}
#endif

#endif
