#ifndef WF_PROVIDER_CREDENTIALS_H
#define WF_PROVIDER_CREDENTIALS_H

#include <webfuse/provider/api.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct wfp_credentials;

typedef void wfp_get_credentials_fn(
    struct wfp_credentials * credentials,
    void * user_data);

extern WFP_API void wfp_credentials_set_type(
    struct wfp_credentials * credentials,
    char const * type);

extern WFP_API void wfp_credentials_add(
    struct wfp_credentials * credentials,
    char const * key,
    char const * value);

#ifdef __cplusplus
}
#endif

#endif
