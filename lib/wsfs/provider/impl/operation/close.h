#ifndef WSFS_PROVIDER_IMPL_OPERATION_CLOSE_H
#define WSFS_PROVIDER_IMPL_OPERATION_CLOSE_H

#include "wsfs/provider/impl/provider.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern void wsfsp_impl_close(
    struct wsfsp_impl_invokation_context * context,
    json_t * params,
    int id);

extern void wsfsp_impl_close_default(
    ino_t inode,
    uint32_t handle,
    int flags,
    void * user_data);

#ifdef __cplusplus
}
#endif

#endif