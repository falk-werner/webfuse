////////////////////////////////////////////////////////////////////////////////
/// \file adapter/server_config.h
/// \brief Server configuration.
////////////////////////////////////////////////////////////////////////////////

#ifndef WF_ADAPTER_SERVER_CONFIG_H
#define WF_ADAPTER_SERVER_CONFIG_H

#include "webfuse/adapter/api.h"
#include "webfuse/adapter/authenticate.h"
#include "webfuse/adapter/mountpoint_factory.h"

#ifdef __cplusplus
extern "C"
{
#endif

//------------------------------------------------------------------------------
/// \struct wf_server_config
/// \brief Holds configuration of webfuse adapter server.
//------------------------------------------------------------------------------
struct wf_server_config;

//------------------------------------------------------------------------------
/// \brief Creates a new server configuration.
///
/// \return newly created server configuration
//------------------------------------------------------------------------------
extern WF_API struct wf_server_config * wf_server_config_create(void);

//------------------------------------------------------------------------------
/// \brief Disposes a server configuration.
///
/// \note Contexts of mounpoint factory and added authenticators are not
///       disposed by default.
///
/// \param config pointer of configuration object
///
/// \see wf_server_config_set_mountpoint_factory
/// \see wf_server_config_add_authenticator
//------------------------------------------------------------------------------
extern WF_API void wf_server_config_dispose(
    struct wf_server_config * config);

//------------------------------------------------------------------------------
/// \brief Sets the mountpoint factory of the configuration.
///
/// The mountpoint factory is called when a authenticated user adds a
/// filesystem.
///
/// \note The user is responsible to manage the lifetime of the mountpoint
///        factory.
///
/// \note A valid configuration needs either a mountpoint or a mounpoint
///       factory.
///
/// \param config pointer of configuration object
/// \param create_mountpoint factory function to create a mountpoint
/// \param user_data context of mountpoint factory
//------------------------------------------------------------------------------
extern WF_API void wf_server_config_set_mountpoint_factory(
    struct wf_server_config * config,
    wf_create_mountpoint_fn * create_mountpoint,
    void * user_data);

//------------------------------------------------------------------------------
/// \brief Sets the path of HTTP servers root context.
///
/// Webfuse adapter server is capable of serving static HTTP context. This
/// function is used to specify the path of HTTP servers root context.
/// If not specified, no HTTP content is served.
///
/// \param config pointer of configuration object
/// \param document_root path to static HTTP content
//------------------------------------------------------------------------------
extern WF_API void wf_server_config_set_documentroot(
    struct wf_server_config * config,
	char const * document_root);

//------------------------------------------------------------------------------
/// \brief Sets the path to the servers private key.
///
/// \note To enable TLS, private key and server certificate must be set.
///       Otherwise, only plain HTTP is supported.
///
/// \param config pointer of configuration object
/// \param key_path path to servers private key (pem file)
//------------------------------------------------------------------------------
extern WF_API void wf_server_config_set_keypath(
    struct wf_server_config * config,
	char const * key_path);

//------------------------------------------------------------------------------
/// \brief Sets path to servers certificate.
///
/// \note To enable TLS, private key and server certificate must be set.
///       Otherwise, only plain HTTP is supported.
///
/// \param config pointer of configuration object
/// \param cert_path path to servers certificate (pem file)
//------------------------------------------------------------------------------
extern WF_API void wf_server_config_set_certpath(
    struct wf_server_config * config,
	char const * cert_path);

//------------------------------------------------------------------------------
/// \brief Sets the virtual hostname of the websockets server.
///
/// \param config pointer of configuration object
/// \param vhost_name virtual hostname of the websockets server
//------------------------------------------------------------------------------
extern WF_API void wf_server_config_set_vhostname(
    struct wf_server_config * config,
	char const * vhost_name);

//------------------------------------------------------------------------------
/// \brief Sets the port number of the websockets server.
///
/// Note: Set port number to 0 to let system choose a free port.
///
/// \param config pointer of configuration object
/// \param port   port number of the websockets server
//------------------------------------------------------------------------------
extern WF_API void wf_server_config_set_port(
    struct wf_server_config * config,
	int port);

//------------------------------------------------------------------------------
/// \brief Adds an authenticator.
///
/// Authenticators are used to authenticate users by some provided credentials.
/// Multiple providers can be specified to support different types of
/// credentials.
///
/// \note Adding multiple providers for the same credentials type results
///       in undefined behavior.
///
/// \note The user is responsible to manage the lifetime of user data.
///
/// \param config pointer to configuration object
/// \param type   type of the credentials the authenticator supports
/// \param authenticate function called to authenticate a user
/// \param user_data context of authenticate function
//------------------------------------------------------------------------------
extern WF_API void wf_server_config_add_authenticator(
    struct wf_server_config * config,
    char const * type,
    wf_authenticate_fn * authenticate,
    void * user_data);

#ifdef __cplusplus
}
#endif

#endif
