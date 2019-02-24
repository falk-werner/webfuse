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

#ifdef __cplusplus
}
#endif

#endif