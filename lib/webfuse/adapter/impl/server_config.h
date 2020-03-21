#ifndef WF_ADAPTER_IMPL_SERVER_CONFIG_H
#define WF_ADAPTER_IMPL_SERVER_CONFIG_H

#include "webfuse/adapter/impl/authenticators.h"
#include "webfuse/adapter/impl/mountpoint_factory.h"

#ifdef __cplusplus
extern "C" {
#endif

struct wf_server_config
{
	char * document_root;
	char * key_path;
	char * cert_path;
	char * vhost_name;
	int port;
	struct wf_impl_authenticators authenticators;
    struct wf_impl_mountpoint_factory mountpoint_factory;
};

extern struct wf_server_config * wf_impl_server_config_create(void);

extern void wf_impl_server_config_dispose(
    struct wf_server_config * config);

extern void wf_impl_server_config_init(
    struct wf_server_config * config);

extern void wf_impl_server_config_cleanup(
    struct wf_server_config * config);

extern void wf_impl_server_config_clone(
	struct wf_server_config * config,
	struct wf_server_config * clone);

extern void wf_impl_server_config_set_mountpoint_factory(
    struct wf_server_config * config,
    wf_create_mountpoint_fn * create_mountpoint,
    void * create_mountpoint_context);

extern void wf_impl_server_config_set_documentroot(
    struct wf_server_config * config,
	char const * document_root);

extern void wf_impl_server_config_set_keypath(
    struct wf_server_config * config,
	char const * key_path);

extern void wf_impl_server_config_set_certpath(
    struct wf_server_config * config,
	char const * cert_path);

extern void wf_impl_server_config_set_vhostname(
    struct wf_server_config * config,
	char const * vhost_name);

extern void wf_impl_server_config_set_port(
    struct wf_server_config * config,
	int port);

extern void wf_impl_server_config_add_authenticator(
    struct wf_server_config * config,
    char const * type,
    wf_authenticate_fn * authenticate,
    void * user_data
);


#ifdef __cplusplus
}
#endif

#endif
