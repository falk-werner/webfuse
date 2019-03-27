#ifndef WF_PROVIDER_IMPL_OPERATION_READDIR_H
#define WF_PROVIDER_IMPL_OPERATION_READDIR_H

#include "webfuse/provider/impl/provider.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern void wfp_impl_respond_readdir(
    struct wfp_request * request,
    struct wfp_dirbuffer * dirbuffer);

extern void wfp_impl_readdir(
    struct wfp_impl_invokation_context * context,
    json_t * params,
    int id);

extern void wfp_impl_readdir_default(
    struct wfp_request * request,
    ino_t directory,
    void * user_data);

#ifdef __cplusplus
}
#endif

#endif