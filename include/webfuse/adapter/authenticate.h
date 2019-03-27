#ifndef WF_ADAPTER_AUTHENTICATE_H
#define WF_ADAPTER_AUTHENTICATE_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_credentials;

typedef bool wf_authenticate_fn(
    struct wf_credentials * credentials,
    void * user_data);

#ifdef __cplusplus
}
#endif

#endif
