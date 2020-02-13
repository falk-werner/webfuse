#ifndef WF_ADAPTER_SERVER_CONFIG_H
#define WF_ADAPTER_SERVER_CONFIG_H

#include "webfuse/adapter/api.h"
#include "webfuse/adapter/authenticate.h"
#include "webfuse/adapter/mountpoint_factory.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_server_config;

extern WF_API struct wf_server_config * wf_server_config_create(void);

extern WF_API void wf_server_config_dispose(
    struct wf_server_config * config);

extern WF_API void wf_server_config_set_mountpoint(
    struct wf_server_config * config,
	char const * mount_point);

extern WF_API void wf_server_config_set_mountpoint_factory(
    struct wf_server_config * config,
    wf_create_mountpoint_fn * create_mountpoint,
    void * user_data);

extern WF_API void wf_server_config_set_documentroot(
    struct wf_server_config * config,
	char const * document_root);

extern WF_API void wf_server_config_set_keypath(
    struct wf_server_config * config,
	char const * key_path);

extern WF_API void wf_server_config_set_certpath(
    struct wf_server_config * config,
	char const * cert_path);

extern WF_API void wf_server_config_set_vhostname(
    struct wf_server_config * config,
	char const * vhost_name);

extern WF_API void wf_server_config_set_port(
    struct wf_server_config * config,
	int port);

extern WF_API void wf_server_config_add_authenticator(
    struct wf_server_config * config,
    char const * type,
    wf_authenticate_fn * authenticate,
    void * user_data
);

#ifdef __cplusplus
}
#endif

#endif
