#ifndef WSFS_PROVIDER_OPERATION_READ_INTERN_H
#define WSFS_PROVIDER_OPERATION_READ_INTERN_H

#include "wsfs/provider/operation/read.h"
#include "wsfs/provider/provider.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern void wsfsp_read(
    struct wsfsp_invokation_context * context,
    json_t * params,
    int id);

extern void wsfsp_read_default(
    struct wsfsp_request * request,
    ino_t inode,
    uint32_t handle,
    size_t offset,
    size_t length,
    void * user_data);

#ifdef __cplusplus
}
#endif

#endif