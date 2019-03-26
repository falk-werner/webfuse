#ifndef WSFS_PROVIDER_IMPL_OPERATION_READDIR_H
#define WSFS_PROVIDER_IMPL_OPERATION_READDIR_H

#include "wsfs/provider/impl/provider.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern void wsfsp_impl_respond_readdir(
    struct wsfsp_request * request,
    struct wsfsp_dirbuffer * dirbuffer);

extern void wsfsp_impl_readdir(
    struct wsfsp_impl_invokation_context * context,
    json_t * params,
    int id);

extern void wsfsp_impl_readdir_default(
    struct wsfsp_request * request,
    ino_t directory,
    void * user_data);

#ifdef __cplusplus
}
#endif

#endif