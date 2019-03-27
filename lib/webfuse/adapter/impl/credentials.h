#ifndef WF_ADAPTER_IMPL_CREDENTIALS_H
#define WF_ADAPTER_IMPL_CREDENTIALS_H

#include <jansson.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_credentials
{
    char * type;
    json_t * data;
};

extern void wf_impl_credentials_init(
    struct wf_credentials * credentials,
    char const * type,
    json_t * data);

extern void wf_impl_credentials_cleanup(
    struct wf_credentials * credentials);

extern char const * wf_impl_credentials_type(
    struct wf_credentials const * credentials);

extern char const * wf_impl_credentials_get(
    struct wf_credentials const * credentials,
    char const * key);

#ifdef __cplusplus
}
#endif

#endif