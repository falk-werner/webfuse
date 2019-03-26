#ifndef WSFS_PROVIDER_IMPL_OPERATION_READ_H
#define WSFS_PROVIDER_IMPL_OPERATION_READ_H

#include "wsfs/provider/impl/provider.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern void wsfsp_impl_respond_read(
    struct wsfsp_request * request,
    char const * data,
    size_t length);

extern void wsfsp_impl_read(
    struct wsfsp_impl_invokation_context * context,
    json_t * params,
    int id);

extern void wsfsp_impl_read_default(
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