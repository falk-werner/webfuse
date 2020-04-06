////////////////////////////////////////////////////////////////////////////////
/// \file provider/client.h
/// \brief Webfuse provider client.
////////////////////////////////////////////////////////////////////////////////

#ifndef WF_PROVIDER_CLIENT_H
#define WF_PROVIDER_CLIENT_H

#include "webfuse/provider/api.h"

#ifdef __cplusplus
extern "C"
{
#endif

//------------------------------------------------------------------------------
/// \struct wfp_client
/// \brief Webfuse provider client.
//------------------------------------------------------------------------------
struct wfp_client;

struct wfp_client_config;

//------------------------------------------------------------------------------
/// \brief Creates a webfuse provider client.
///
/// \note Client configuration is not managed by the client.
///
/// \param config pointer to client configuration.
/// \return newly created client or NULL in case of an error.
//------------------------------------------------------------------------------
extern WFP_API struct wfp_client * wfp_client_create(
    struct wfp_client_config * config);

//------------------------------------------------------------------------------
/// \brief Connects the client to a remote webfuse adapter server.
///
/// \note This call starts to establish a connection. A callback is invoked,
///       when the connection is estanlished.
///
/// \param client pointer to client
/// \param url URL of remote webfuse adapter server
///
/// \see wfp_connected_fn
/// \see wfp_client_config_set_onconnected
//------------------------------------------------------------------------------
extern WFP_API void wfp_client_connect(
    struct wfp_client * client,
    char const * url);

//------------------------------------------------------------------------------
/// \brief Disconnects a connected client.
///
/// \note This call starts to disconnect the connection. A callback is invoked
///       when conntection is disconnected.
///
/// \param client pointer to client
///
/// \see wfp_disconnected_fn
/// \see wfp_client_config_set_ondisconnected
//------------------------------------------------------------------------------
extern WFP_API void wfp_client_disconnect(
    struct wfp_client * client);

//------------------------------------------------------------------------------
/// \brief Disposes a client.
///
/// \note Client configuration is not managed by client.
///
/// \param client pointer to client
//------------------------------------------------------------------------------
extern WFP_API void wfp_client_dispose(
    struct wfp_client * client);

//------------------------------------------------------------------------------
/// \brief Triggers the client.
///
/// This function must be invoked in a loop while the client is running. It
/// makes the server wait for the next event and processes it.
///
/// \param client pointer to client
///
/// \see wfp_client_interrupt
//------------------------------------------------------------------------------
extern WFP_API void wfp_client_service(
    struct wfp_client * client);

//------------------------------------------------------------------------------
/// \brief interrupt wfp_client_service
///
/// This function can be called from another thread.
///
/// \param client pointer to client
///
/// \see wfp_client_service
//------------------------------------------------------------------------------
extern WFP_API void wfp_client_interrupt(
    struct wfp_client * client);

#ifdef __cplusplus
}
#endif


#endif
