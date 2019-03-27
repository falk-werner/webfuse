#ifndef WF_PROVIDER_IMPL_OPERATION_READ_H
#define WF_PROVIDER_IMPL_OPERATION_READ_H

#include "webfuse/provider/impl/provider.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern void wfp_impl_respond_read(
    struct wfp_request * request,
    char const * data,
    size_t length);

extern void wfp_impl_read(
    struct wfp_impl_invokation_context * context,
    json_t * params,
    int id);

extern void wfp_impl_read_default(
    struct wfp_request * request,
    ino_t inode,
    uint32_t handle,
    size_t offset,
    size_t length,
    void * user_data);

#ifdef __cplusplus
}
#endif

#endif