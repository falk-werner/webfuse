#ifndef WF_PROVIDER_CLIENT_CONFIG_H
#define WF_PROVIDER_CLIENT_CONFIG_H

#include <webfuse/provider/api.h>

#include <webfuse/provider/operation/lookup.h>
#include <webfuse/provider/operation/getattr.h>
#include <webfuse/provider/operation/readdir.h>
#include <webfuse/provider/operation/open.h>
#include <webfuse/provider/operation/close.h>
#include <webfuse/provider/operation/read.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct wfp_client_config;

typedef void wfp_connected_fn(
    void * user_data);

typedef void wfp_disconnected_fn(
    void * user_data);

typedef void wfp_ontimer_fn(
    void * user_data);

extern WFP_API struct wfp_client_config * wfp_client_config_create(void);

extern WFP_API void wfp_client_config_dispose(
    struct wfp_client_config * config);

extern WFP_API void wfp_client_config_set_userdata(
    struct wfp_client_config * config,
    void * user_data);

extern WFP_API void wfp_client_config_set_keypath(
    struct wfp_client_config * config,
    char const * key_path);

extern WFP_API void wfp_client_config_set_certpath(
    struct wfp_client_config * config,
    char const * cert_path);

extern WFP_API void wfp_client_config_set_onconnected(
    struct wfp_client_config * config,
    wfp_connected_fn * handler);

extern WFP_API void wfp_client_config_set_ondisconnected(
    struct wfp_client_config * config,
    wfp_disconnected_fn * handler);

extern WFP_API void wfp_client_config_set_ontimer(
    struct wfp_client_config * config,
    wfp_ontimer_fn * handler);


extern WFP_API void wfp_client_config_set_onlookup(
    struct wfp_client_config * config,
    wfp_lookup_fn * handler);

extern WFP_API void wfp_client_config_set_ongetattr(
    struct wfp_client_config * config,
    wfp_getattr_fn * handler);

extern WFP_API void wfp_client_config_set_onreaddir(
    struct wfp_client_config * config,
    wfp_readdir_fn * handler);

extern WFP_API void wfp_client_config_set_onopen(
    struct wfp_client_config * config,
    wfp_open_fn * handler);

extern WFP_API void wfp_client_config_set_onclose(
    struct wfp_client_config * config,
    wfp_close_fn * handler);

extern WFP_API void wfp_client_config_set_onread(
    struct wfp_client_config * config,
    wfp_read_fn * handler);

#ifdef __cplusplus
}
#endif


#endif
