#ifndef WF_PROVIDER_IMPL_CREDENTIALS_H
#define WF_PROVIDER_IMPL_CREDENTIALS_H

#include "webfuse/provider/credentials.h"
#include <jansson.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct wfp_credentials
{
    char * type;
    json_t * contents;
};

extern void  wfp_impl_credentials_init(
    struct wfp_credentials * credentials);

extern void wfp_impl_credentials_cleanup(
    struct wfp_credentials * credentials);

extern void wfp_impl_credentials_set_type(
    struct wfp_credentials * credentials,
    char const * type);

extern void wfp_impl_credentials_add(
    struct wfp_credentials * credentials,
    char const * key,
    char const * value);

extern char const * wfp_impl_credentials_get_type(
    struct wfp_credentials * credentials);

extern json_t * wfp_impl_credentials_get(
    struct wfp_credentials * credentials);

#ifdef __cplusplus
}
#endif

#endif
