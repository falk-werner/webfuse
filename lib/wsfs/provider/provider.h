#ifndef WSFS_PROVIDER_PROVIDER_H
#define WSFS_PROVIDER_PROVIDER_H

#include "wsfs/provider/client_config.h"
#include "wsfs/provider/request.h"
#include <wsfs/provider/operation/lookup.h>
#include <wsfs/provider/operation/getattr.h>
#include <wsfs/provider/operation/readdir.h>
#include <wsfs/provider/operation/open.h>
#include <wsfs/provider/operation/close.h>
#include <wsfs/provider/operation/read.h>

#include <jansson.h>

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

struct wsfsp_invokation_context
{
    struct wsfsp_provider * provider;
    void * user_data;
    struct wsfsp_request * request;
};

#ifdef __cplusplus
extern "C"
{
#endif

extern void wsfsp_provider_init(
    struct wsfsp_provider * provider);

extern void wsfsp_provider_init_from_prototype(
    struct wsfsp_provider * provider,
    struct wsfsp_provider const * prototype);


extern void wsfsp_provider_invoke(
    struct wsfsp_invokation_context * context,
    json_t * request);

extern void wsfsp_connected_default(
    void * user_data);

extern void wsfsp_disconnected_default(
    void * user_data);

extern void wsfsp_ontimer_default(
    void * user_data);

#ifdef __cplusplus    
}
#endif

#endif