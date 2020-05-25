////////////////////////////////////////////////////////////////////////////////
/// \file provider/client_config.h
/// \brief Client configuration of webfuse provider.
////////////////////////////////////////////////////////////////////////////////

#ifndef WF_PROVIDER_CLIENT_CONFIG_H
#define WF_PROVIDER_CLIENT_CONFIG_H

#include <webfuse/provider/api.h>

#include <webfuse/provider/operation/lookup.h>
#include <webfuse/provider/operation/getattr.h>
#include <webfuse/provider/operation/readdir.h>
#include <webfuse/provider/operation/open.h>
#include <webfuse/provider/operation/close.h>
#include <webfuse/provider/operation/read.h>
#include <webfuse/provider/credentials.h>

#ifdef __cplusplus
extern "C"
{
#endif

//------------------------------------------------------------------------------
/// \struct wfp_client_config
/// \brief Provider client configuration object.
///
/// Holds configuration of webfuse provider client.
//------------------------------------------------------------------------------
struct wfp_client_config;

//------------------------------------------------------------------------------
/// \brief Callback to signal when the client's connection is established.
///
/// \param user_data user defined context
//------------------------------------------------------------------------------
typedef void wfp_connected_fn(
    void * user_data);

//------------------------------------------------------------------------------
/// \brief Callback to signal when a client's connection is disconnected.
///
/// \param user_data user defined context
//------------------------------------------------------------------------------
typedef void wfp_disconnected_fn(
    void * user_data);

//------------------------------------------------------------------------------
/// \brief Creates a new client configuration.
///
/// \return newly created client configuration
//------------------------------------------------------------------------------
extern WFP_API struct wfp_client_config * wfp_client_config_create(void);

//------------------------------------------------------------------------------
/// \brief Disposes a client configuration.
///
/// \note The user defined context is not managed by the client configuration.
///
/// \param config pointer to client configuration
//------------------------------------------------------------------------------
extern WFP_API void wfp_client_config_dispose(
    struct wfp_client_config * config);

//------------------------------------------------------------------------------
/// \brief Sets a user defined context.
///
/// \note The user is responsible to manage the lifetime of user data.
///
/// \param config pointer to client configuration
/// \param user_data user defined context
//------------------------------------------------------------------------------
extern WFP_API void wfp_client_config_set_userdata(
    struct wfp_client_config * config,
    void * user_data);

//------------------------------------------------------------------------------
/// \brief Sets the path to clients private key.
///
/// \note To enable TLS both, private key and certificate, must be specified.
///       Otherwise, TLS is not used.
///
/// \param config pointer to client configuration
/// \param key_path path of clients private key (pem file)
//------------------------------------------------------------------------------
extern WFP_API void wfp_client_config_set_keypath(
    struct wfp_client_config * config,
    char const * key_path);

//------------------------------------------------------------------------------
/// \brief Sets the path of clients certificate.
///
/// \note To enable TLS both, private key and certificate, must be specified.
///       Otherwise, TLS is not used.
///
/// \param config pointer to client configuration
/// \param cert_path path of the clients certificate (pem file)
//------------------------------------------------------------------------------
extern WFP_API void wfp_client_config_set_certpath(
    struct wfp_client_config * config,
    char const * cert_path);

//------------------------------------------------------------------------------
/// \brief Sets the path of ca file to verify servers.
///
/// \note To enable TLS both, private key and certificate, must be specified.
///       Otherwise, TLS is not used.
///
/// \param config pointer to client configuration
/// \param ca_filepath path of the ca file (pem file)
//------------------------------------------------------------------------------
extern WFP_API void wfp_client_config_set_ca_filepath(
    struct wfp_client_config * config,
    char const * ca_filepath);

//------------------------------------------------------------------------------
/// \brief Sets the onconnected handler.
///
/// The handler is invoked, when the client's conntection is established.
///
/// \param config pointer to client configuration
/// \param handler pointer to handler
//------------------------------------------------------------------------------
extern WFP_API void wfp_client_config_set_onconnected(
    struct wfp_client_config * config,
    wfp_connected_fn * handler);

//------------------------------------------------------------------------------
/// \brief Sets ondisconnected handler
///
/// The handler is invoked, when the client's conntection is lost.
///
/// \param config pointer to client configuration
/// \param handler pointer to handler
//------------------------------------------------------------------------------
extern WFP_API void wfp_client_config_set_ondisconnected(
    struct wfp_client_config * config,
    wfp_disconnected_fn * handler);

//------------------------------------------------------------------------------
/// \brief Sets onlookup handler.
///
/// The handler is invoked, when the identifier of a file is requested.
///
/// \param config pointer to client configuration
/// \param handler pointer to handler
///
/// \see wfp_lookup_fn
//------------------------------------------------------------------------------
extern WFP_API void wfp_client_config_set_onlookup(
    struct wfp_client_config * config,
    wfp_lookup_fn * handler);

//------------------------------------------------------------------------------
/// \brief Sets ongetattr handler.
///
/// The handler is invoked, when attributes of a file are requested.
///
/// \param config pointer to client configuration
/// \param handler pointer to handler
///
/// \see wfp_getattr_fn
//------------------------------------------------------------------------------
extern WFP_API void wfp_client_config_set_ongetattr(
    struct wfp_client_config * config,
    wfp_getattr_fn * handler);

//------------------------------------------------------------------------------
/// \brief Sets onreaddir handler.
///
/// The handler is invoked, when the contents of directory are requested-
///
/// \param config pointer to client configuration
/// \param handler pointer to handler
///
/// \see wfp_readdir_fn
//------------------------------------------------------------------------------
extern WFP_API void wfp_client_config_set_onreaddir(
    struct wfp_client_config * config,
    wfp_readdir_fn * handler);

//------------------------------------------------------------------------------
/// \brief Sets onopen handler.
///
/// The handler is invoked, whe a file should be opened.
///
/// \param config pointer to client configuration
/// \param handler pointer to handler
///
/// \see wfp_open_fn
//------------------------------------------------------------------------------
extern WFP_API void wfp_client_config_set_onopen(
    struct wfp_client_config * config,
    wfp_open_fn * handler);

//------------------------------------------------------------------------------
/// \brief Sets onclose handler.
///
/// The handler is invoked, when a file is closed.
///
/// \param config pointer to client configuration
/// \param handler pointer to handler
///
/// \see wfp_close_fn
//------------------------------------------------------------------------------
extern WFP_API void wfp_client_config_set_onclose(
    struct wfp_client_config * config,
    wfp_close_fn * handler);

//------------------------------------------------------------------------------
/// \brief Sets onread handler.
///
/// The handler is invoked, when a files content is requested.
///
/// \param config pointer to client configuration
/// \param handler pointer to handler
///
/// \see wfp_read_fn
//------------------------------------------------------------------------------
extern WFP_API void wfp_client_config_set_onread(
    struct wfp_client_config * config,
    wfp_read_fn * handler);

//------------------------------------------------------------------------------
/// \brief Enabled authentication.
///
/// \param config pointer to client configuration
/// \param get_credentials pointer to function providing credentials when 
//         needed.
//------------------------------------------------------------------------------
extern WFP_API void wfp_client_config_enable_authentication(
    struct wfp_client_config * config,
    wfp_get_credentials_fn * get_credentials);

#ifdef __cplusplus
}
#endif


#endif
