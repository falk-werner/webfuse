#ifndef WSFSP_PROVIDER_INTERN_H
#define WSFSP_PROVIDER_INTERN_H

#include "wsfsp/provider.h"
#include <jansson.h>

struct wsfsp_invokation_context
{
    struct wsfsp_provider * provider;
    void * user_data;
    struct wsfsp_request * request;
};

#ifdef __cplusplus
extern "C"
{
#endif

extern void wsfsp_provider_invoke(
    struct wsfsp_invokation_context * context,
    json_t * request);

extern void wsfsp_connected_default(
    void * user_data);

extern void wsfsp_disconnected_default(
    void * user_data);

extern void wsfsp_ontimer_default(
    void * user_data);

#ifdef __cplusplus    
}
#endif

#endif