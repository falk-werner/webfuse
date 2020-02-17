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
/// \struct wfp_provider
/// \brief Provider.
///
/// \todo How is a user supposed to get a provider's instance?
//------------------------------------------------------------------------------
struct wfp_provider;

//------------------------------------------------------------------------------
/// \struct lws_protocols
/// \brief Forward declaration of libwebsockets protocols structure.
//------------------------------------------------------------------------------
struct lws_protocols;

//------------------------------------------------------------------------------
/// \brief Creates a new webfuse provider client protocol.
///
/// \note The user is responsible to manage lifetime of \arg user_data.
///
/// \todo How is a user supposed to get a provider's instance?
///
/// \param provider pointer to provider
/// \param user_data user defined context
/// \return newly created protocol
//------------------------------------------------------------------------------
extern WFP_API struct wfp_client_protocol * wfp_client_protocol_create(
    struct wfp_provider const * provider,
    void * user_data);

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

#ifdef __cplusplus
}
#endif



#endif
