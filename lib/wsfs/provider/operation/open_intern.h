#ifndef WSFS_PROVIDER_OPERATION_OPEN_INTERN_H
#define WSFS_PROVIDER_OPERATION_OPEN_INTERN_H

#include "wsfs/provider/operation/open.h"
#include "wsfs/provider/provider_intern.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern void wsfsp_open(
    struct wsfsp_invokation_context * context,
    json_t * params,
    int id);

extern void wsfsp_open_default(
    struct wsfsp_request * request,
    ino_t inode,
    int flags,
    void * user_data);

#ifdef __cplusplus
}
#endif

#endif