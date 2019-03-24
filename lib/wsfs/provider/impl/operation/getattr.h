#ifndef WSFS_PROVIDER_IMPL_OPERATION_GETATTR_H
#define WSFS_PROVIDER_IMPL_OPERATION_GETATTR_H

#include "wsfs/provider/impl/provider.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern void wsfsp_impl_respond_getattr(
    struct wsfsp_request * request,
    struct stat const * stat);

extern void wsfsp_impl_getattr(
    struct wsfsp_impl_invokation_context * context,
    json_t * params,
    int id);

extern void wsfsp_impl_getattr_default(
    struct wsfsp_request * request,
    ino_t inode,
    void * user_data);

#ifdef __cplusplus
}
#endif

#endif