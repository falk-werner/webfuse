#ifndef WSFS_PROVIDER_IMPL_OPERATION_OPEN_H
#define WSFS_PROVIDER_IMPL_OPERATION_OPEN_H

#include "wsfs/provider/impl/provider.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern void wsfsp_impl_respond_open(
    struct wsfsp_request * request,
    uint32_t handle);

extern void wsfsp_impl_open(
    struct wsfsp_impl_invokation_context * context,
    json_t * params,
    int id);

extern void wsfsp_impl_open_default(
    struct wsfsp_request * request,
    ino_t inode,
    int flags,
    void * user_data);

#ifdef __cplusplus
}
#endif

#endif