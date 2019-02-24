#ifndef WSFSP_OPERATION_LOOKUP_INTERN_H
#define WSFSP_OPERATION_LOOKUP_INTERN_H

#include "wsfsp/operation/lookup.h"
#include "wsfsp/provider_intern.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern void wsfsp_lookup(
    struct wsfsp_invokation_context * context,
    json_t * params,
    int id);

#ifdef __cplusplus
}
#endif

#endif