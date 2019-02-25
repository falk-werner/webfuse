#ifndef WSFSP_OPERATION_READDIR_INTERN_H
#define WSFSP_OPERATION_READDIR_INTERN_H

#include "wsfs/provider/operation/readdir.h"
#include "wsfs/provider/provider_intern.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern void wsfsp_readdir(
    struct wsfsp_invokation_context * context,
    json_t * params,
    int id);

extern void wsfsp_readdir_default(
    struct wsfsp_request * request,
    ino_t directory,
    void * user_data);

#ifdef __cplusplus
}
#endif

#endif