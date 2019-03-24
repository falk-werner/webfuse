#ifndef WSFS_PROVIDER_IMPL_PROVIDER_H
#define WSFS_PROVIDER_IMPL_PROVIDER_H

#include <jansson.h>
#include "wsfs/provider/client_config.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wsfsp_provider
{
    wsfsp_connected_fn * connected;
    wsfsp_disconnected_fn * disconnected;
    wsfsp_ontimer_fn * ontimer;
    wsfsp_lookup_fn * lookup;
    wsfsp_getattr_fn * getattr;
    wsfsp_readdir_fn * readdir;
    wsfsp_open_fn * open;
    wsfsp_close_fn * close;
    wsfsp_read_fn * read;
};

struct wsfsp_impl_invokation_context
{
    struct wsfsp_provider * provider;
    void * user_data;
    struct wsfsp_request * request;
};

extern void wsfsp_impl_provider_init(
    struct wsfsp_provider * provider);

extern void wsfsp_impl_provider_init_from_prototype(
    struct wsfsp_provider * provider,
    struct wsfsp_provider const * prototype);


extern void wsfsp_impl_provider_invoke(
    struct wsfsp_impl_invokation_context * context,
    json_t * request);

extern void wsfsp_impl_connected_default(
    void * user_data);

extern void wsfsp_impl_disconnected_default(
    void * user_data);

extern void wsfsp_impl_ontimer_default(
    void * user_data);

#ifdef __cplusplus    
}
#endif

#endif