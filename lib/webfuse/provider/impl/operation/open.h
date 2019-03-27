#ifndef WF_PROVIDER_IMPL_OPERATION_OPEN_H
#define WF_PROVIDER_IMPL_OPERATION_OPEN_H

#include "webfuse/provider/impl/provider.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern void wfp_impl_respond_open(
    struct wfp_request * request,
    uint32_t handle);

extern void wfp_impl_open(
    struct wfp_impl_invokation_context * context,
    json_t * params,
    int id);

extern void wfp_impl_open_default(
    struct wfp_request * request,
    ino_t inode,
    int flags,
    void * user_data);

#ifdef __cplusplus
}
#endif

#endif