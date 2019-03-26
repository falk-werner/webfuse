#ifndef WF_PROVIDER_IMPL_OPERATION_CLOSE_H
#define WF_PROVIDER_IMPL_OPERATION_CLOSE_H

#include "webfuse/provider/impl/provider.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern void wfp_impl_close(
    struct wfp_impl_invokation_context * context,
    json_t * params,
    int id);

extern void wfp_impl_close_default(
    ino_t inode,
    uint32_t handle,
    int flags,
    void * user_data);

#ifdef __cplusplus
}
#endif

#endif