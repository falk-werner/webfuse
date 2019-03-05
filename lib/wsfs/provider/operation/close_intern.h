#ifndef WSFS_PROVIDER_OPERATION_CLOSE_INTERN_H
#define WSFS_PROVIDER_OPERATION_CLOSE_INTERN_H

#include "wsfs/provider/operation/close.h"
#include "wsfs/provider/provider.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern void wsfsp_close(
    struct wsfsp_invokation_context * context,
    json_t * params,
    int id);

extern void wsfsp_close_default(
    ino_t inode,
    uint32_t handle,
    int flags,
    void * user_data);

#ifdef __cplusplus
}
#endif

#endif