#ifndef WF_ADAPTER_CLIENT_CREDENTIALS_H
#define WF_ADAPTER_CLIENT_CREDENTIALS_H

#include "webfuse/adapter/api.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_client_credentials;

extern WF_API void
wf_client_credentials_set_type(
    struct wf_client_credentials * credentials,
    char const * type);

extern WF_API void
wf_client_credentials_add(
    struct wf_client_credentials * credentials,
    char const * key,
    char const * value);

#ifdef __cplusplus
}
#endif

#endif
