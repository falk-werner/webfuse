#ifndef WF_ADAPTER_CLIENT_TLSCONFIG_H
#define WF_ADAPTER_CLIENT_TLSCONFIG_H

#include <webfuse/adapter/api.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_client_tlsconfig;

extern WF_API void
wf_client_tlsconfig_set_keypath(
    struct wf_client_tlsconfig * config,
    char const * key_path);

extern WF_API void
wf_client_tlsconfig_set_certpath(
    struct wf_client_tlsconfig * config,
    char const * cert_path);

extern WF_API void
wf_client_tlsconfig_set_cafilepath(
    struct wf_client_tlsconfig * config,
    char const * cafile_path);

#ifdef __cplusplus
}
#endif

#endif
