#ifndef WSFS_PROVIDER_OPERATION_GETATTR_INTERN_H
#define WSFS_PROVIDER_OPERATION_GETATTR_INTERN_H

#include "wsfs/provider/operation/getattr.h"
#include "wsfs/provider/provider_intern.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern void wsfsp_getattr(
    struct wsfsp_invokation_context * context,
    json_t * params,
    int id);

extern void wsfsp_getattr_default(
    struct wsfsp_request * request,
    ino_t inode,
    void * user_data);

#ifdef __cplusplus
}
#endif

#endif