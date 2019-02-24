#ifndef WSFSP_OPERATION_READDIR_INTERN_H
#define WSFSP_OPERATION_READDIR_INTERN_H

#include "wsfsp/operation/readdir.h"
#include "wsfsp/provider_intern.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern void wsfsp_readdir(
    struct wsfsp_invokation_context * context,
    json_t * params,
    int id);

#ifdef __cplusplus
}
#endif

#endif