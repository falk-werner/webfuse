#ifndef WSFSP_OPERATION_CLOSE_H
#define WSFSP_OPERATION_CLOSE_H

#ifndef __cplusplus
#include <inttypes.h>
#else
#include <cinttypes>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "wsfs/provider/api.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef void wsfsp_close_fn(
    ino_t inode,
    uint32_t handle,
    int flags,
    void * user_data);

#ifdef __cplusplus
}
#endif

#endif
