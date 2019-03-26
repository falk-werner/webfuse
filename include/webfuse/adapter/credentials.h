#ifndef WF_ADAPTER_CREDENTIALS_H
#define WF_ADAPTER_CREDENTIALS_H

#include "webfuse/adapter/api.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_credentials;

extern WF_API char const * wf_credentials_type(
    struct wf_credentials const * credentials);

extern WF_API char const * wf_credentials_get(
    struct wf_credentials const * credentials,
    char const * key);

#ifdef __cplusplus
}
#endif


#endif
