#ifndef WSFSP_OPERATION_READ_INTERN_H
#define WSFSP_OPERATION_READ_INTERN_H

#include "wsfsp/operation/read.h"
#include "wsfsp/provider_intern.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern void wsfsp_read(
    struct wsfsp_invokation_context * context,
    json_t * params,
    int id);

extern void wsfsp_read_default(
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