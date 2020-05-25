////////////////////////////////////////////////////////////////////////////////
/// \file provider/client_protocol.h
/// \brief Provides low level access to libwebsockets protocol.
///
/// By default, libwebfuse encapsulates libwebsockets protocol by \ref
/// wfp_client. But sometimes it might come in handy to have access to 
/// libwebsockets protocol. This allows to integrate libwebfuse in existing
/// libwebsockets applications.
////////////////////////////////////////////////////////////////////////////////

#ifndef WF_PROVIDER_CLIENT_PROTOCOL_H
#define WF_PROVIDER_CLIENT_PROTOCOL_H

#include "webfuse/provider/api.h"

#ifdef __cplusplus
extern "C"
{
#endif

//------------------------------------------------------------------------------
/// \struct wfp_client_protocol
/// \brief Opaque webfuse client protocol..
//------------------------------------------------------------------------------
struct wfp_client_protocol;

//------------------------------------------------------------------------------
/// \struct lws_protocols
/// \brief Forward declaration of libwebsockets protocols structure.
//------------------------------------------------------------------------------
struct lws_protocols;

//------------------------------------------------------------------------------
/// \struct lws_context
/// \brief Forward declaration of libwebsockets context structure.
//------------------------------------------------------------------------------
struct lws_context;

//------------------------------------------------------------------------------
/// \struct wfp_client_config
/// \copydoc wfp_client_config
//------------------------------------------------------------------------------
struct wfp_client_config;

//------------------------------------------------------------------------------
/// \brief Creates a new webfuse provider client protocol.
///
/// \note The user is responsible to manage lifetime of \arg config.
///
/// \note TLS configuration is ignored, since TLS is managed by libwebsockets.
///
/// \param config pointer to client config
/// \return newly created protocol
//------------------------------------------------------------------------------
extern WFP_API struct wfp_client_protocol * wfp_client_protocol_create(
    struct wfp_client_config const * config);

//------------------------------------------------------------------------------
/// \brief Disposes a protocol.
///
/// \note The user defined context is not managed by the protocol.
///
/// \param protocol pointer to protocol.
//------------------------------------------------------------------------------
extern WFP_API void wfp_client_protocol_dispose(
    struct wfp_client_protocol * protocol);

//------------------------------------------------------------------------------
/// \brief Initialized libwebsockets protocol structure.
///
/// \param protocol pointer to protocol
/// \param lws_protocol pointer to libwebsockets protocol structure.
//------------------------------------------------------------------------------
extern WFP_API void wfp_client_protocol_init_lws(
    struct wfp_client_protocol * protocol,
    struct lws_protocols * lws_protocol);


//------------------------------------------------------------------------------
/// \brief Connects the protocol to a remote webfuse adapter server.
///
/// \note This call starts to establish a connection. A callback is invoked,
///       when the connection is estanlished.
///
/// \param protocol pointer to protocol
/// \param context lws context
/// \param url URL of remote webfuse adapter server
///
/// \see wfp_connected_fn
/// \see wfp_client_config_set_onconnected
//------------------------------------------------------------------------------
extern WFP_API void wfp_client_protocol_connect(
    struct wfp_client_protocol * protocol,
    struct lws_context * context,
    char const * url);

//------------------------------------------------------------------------------
/// \brief Disconnects the protocol from a remote webfuse adapter server.
///
/// \note This call starts to disconnect. A callback is invoked,
///       when the connection is estanlished.
///
/// \param protocol pointer to protocol
///
/// \see wfp_connected_fn
/// \see wfp_client_config_set_ondisconnected
//------------------------------------------------------------------------------
extern WFP_API void wfp_client_protocol_disconnect(
    struct wfp_client_protocol * protocol);

#ifdef __cplusplus
}
#endif



#endif
