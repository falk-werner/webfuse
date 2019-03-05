#ifndef WSFS_PROVIDER_CLIENT_CONFIG_H
#define WSFS_PROVIDER_CLIENT_CONFIG_H

#include <wsfs/provider/api.h>

#include <wsfs/provider/operation/lookup.h>
#include <wsfs/provider/operation/getattr.h>
#include <wsfs/provider/operation/readdir.h>
#include <wsfs/provider/operation/open.h>
#include <wsfs/provider/operation/close.h>
#include <wsfs/provider/operation/read.h>

struct wsfsp_client_config;

typedef void wsfsp_connected_fn(
    void * user_data);

typedef void wsfsp_disconnected_fn(
void * user_data);

typedef void wsfsp_ontimer_fn(
    void * user_data);

#ifdef __cplusplus
extern "C"
{
#endif

extern WSFSP_API struct wsfsp_client_config * wsfsp_client_config_create(void);

extern WSFSP_API void wsfsp_client_config_dispose(
    struct wsfsp_client_config * config);

extern WSFSP_API void wsfsp_client_config_set_userdata(
    struct wsfsp_client_config * config,
    void * user_data);

extern WSFSP_API void wsfsp_client_config_set_keypath(
    struct wsfsp_client_config * config,
    char const * key_path);

extern WSFSP_API void wsfsp_client_config_set_certpath(
    struct wsfsp_client_config * config,
    char const * cert_path);

extern WSFSP_API void wsfsp_client_config_set_onconnected(
    struct wsfsp_client_config * config,
    wsfsp_connected_fn * handler);

extern WSFSP_API void wsfsp_client_config_set_ondisconnected(
    struct wsfsp_client_config * config,
    wsfsp_disconnected_fn * handler);

extern WSFSP_API void wsfsp_client_config_set_ontimer(
    struct wsfsp_client_config * config,
    wsfsp_ontimer_fn * handler);


extern WSFSP_API void wsfsp_client_config_set_onlookup(
    struct wsfsp_client_config * config,
    wsfsp_lookup_fn * handler);

extern WSFSP_API void wsfsp_client_config_set_ongetattr(
    struct wsfsp_client_config * config,
    wsfsp_getattr_fn * handler);

extern WSFSP_API void wsfsp_client_config_set_onreaddir(
    struct wsfsp_client_config * config,
    wsfsp_readdir_fn * handler);

extern WSFSP_API void wsfsp_client_config_set_onopen(
    struct wsfsp_client_config * config,
    wsfsp_open_fn * handler);

extern WSFSP_API void wsfsp_client_config_set_onclose(
    struct wsfsp_client_config * config,
    wsfsp_close_fn * handler);

extern WSFSP_API void wsfsp_client_config_set_onread(
    struct wsfsp_client_config * config,
    wsfsp_read_fn * handler);

#ifdef __cplusplus
}
#endif


#endif
