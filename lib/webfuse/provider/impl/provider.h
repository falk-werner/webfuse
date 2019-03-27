#ifndef WF_PROVIDER_IMPL_PROVIDER_H
#define WF_PROVIDER_IMPL_PROVIDER_H

#include <jansson.h>
#include "webfuse/provider/client_config.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wfp_provider
{
    wfp_connected_fn * connected;
    wfp_disconnected_fn * disconnected;
    wfp_ontimer_fn * ontimer;
    wfp_lookup_fn * lookup;
    wfp_getattr_fn * getattr;
    wfp_readdir_fn * readdir;
    wfp_open_fn * open;
    wfp_close_fn * close;
    wfp_read_fn * read;
};

struct wfp_impl_invokation_context
{
    struct wfp_provider * provider;
    void * user_data;
    struct wfp_request * request;
};

extern void wfp_impl_provider_init(
    struct wfp_provider * provider);

extern void wfp_impl_provider_init_from_prototype(
    struct wfp_provider * provider,
    struct wfp_provider const * prototype);


extern void wfp_impl_provider_invoke(
    struct wfp_impl_invokation_context * context,
    json_t * request);

extern void wfp_impl_connected_default(
    void * user_data);

extern void wfp_impl_disconnected_default(
    void * user_data);

extern void wfp_impl_ontimer_default(
    void * user_data);

#ifdef __cplusplus    
}
#endif

#endif