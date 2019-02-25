#ifndef WSFSP_OPERATION_LOOKUP_INTERN_H
#define WSFSP_OPERATION_LOOKUP_INTERN_H

#include "wsfs/provider/operation/lookup.h"
#include "wsfs/provider/provider_intern.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern void wsfsp_lookup(
    struct wsfsp_invokation_context * context,
    json_t * params,
    int id);

extern void wsfsp_lookup_default(
    struct wsfsp_request * request,
    ino_t parent,
    char const * name,
    void * user_data);

#ifdef __cplusplus
}
#endif

#endif