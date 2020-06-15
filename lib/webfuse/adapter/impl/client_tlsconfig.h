#ifndef WF_ADAPTER_IMPL_CLIENT_TLSCONFIG_H
#define WF_ADAPTER_IMPL_CLIENT_TLSCONFIG_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_client_tlsconfig
{
    char * key_path;
    char * cert_path;
    char * cafile_path;
};

extern void
wf_impl_client_tlsconfig_init(
    struct wf_client_tlsconfig * config);

extern void
wf_impl_client_tlsconfig_cleanup(
    struct wf_client_tlsconfig * config);

extern void
wf_impl_client_tlsconfig_set_keypath(
    struct wf_client_tlsconfig * config,
    char const * key_path);

extern void
wf_impl_client_tlsconfig_set_certpath(
    struct wf_client_tlsconfig * config,
    char const * cert_path);

extern void
wf_impl_client_tlsconfig_set_cafilepath(
    struct wf_client_tlsconfig * config,
    char const * cafile_path);

extern bool
wf_impl_client_tlsconfig_isset(
    struct wf_client_tlsconfig const * config);


#ifdef __cplusplus
}
#endif

#endif
