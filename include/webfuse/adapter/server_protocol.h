////////////////////////////////////////////////////////////////////////////////
/// \file adapter/server_protocol.h
/// \brief Provides low level access to libwebsockets protocol.
///
/// By default, libwebfuse encapsulates libwebsockets protocol by \ref
/// wf_server. But sometimes it might come in handy to have access to 
/// libwebsockets protocol. This allows to integrate libwebfuse in existing
/// libwebsockets applications.
////////////////////////////////////////////////////////////////////////////////

#ifndef WF_ADAPTER_SERVER_PROTOCOL_H
#define WF_ADAPTER_SERVER_PROTOCOL_H

#include <webfuse/adapter/api.h>
#include <webfuse/adapter/authenticate.h>
#include <webfuse/adapter/mountpoint_factory.h>

#ifdef __cplusplus
extern "C"
{
#endif

//------------------------------------------------------------------------------
/// \struct wf_server_protocol
/// \brief Opaque webfuse server protocol.
//------------------------------------------------------------------------------
struct wf_server_protocol;

//------------------------------------------------------------------------------
/// \struct lws_protocols
/// \brief Forward declaration of libwebsockets protocols structure.
//------------------------------------------------------------------------------
struct lws_protocols;

//------------------------------------------------------------------------------
/// \brief Creates a new protocol by a mountpoint factory.
///
/// \note This function might be renamed in future releases.
///
/// \note The user is responsible to manage the lifetime of mountpoint factory.
///
/// \param create_mountpoint factory function to create mountpoints
/// \param create_mountpoint_context context of mountpoint factory
//------------------------------------------------------------------------------
extern WF_API struct wf_server_protocol * wf_server_protocol_create(
    wf_create_mountpoint_fn * create_mountpoint,
    void * create_mountpoint_context);

//------------------------------------------------------------------------------
/// \brief Disposes a protocol.
///
/// \note Contexts of mountpoint factory and added authenticators are not
///       managed by dispose.
///
/// \param protocol pointer to protocol
//------------------------------------------------------------------------------
extern WF_API void wf_server_protocol_dispose(
    struct wf_server_protocol * protocol);

//------------------------------------------------------------------------------
/// \brief Intializes a libwebsockets protocol structure.
///
/// \param protocol pointer to protocol
/// \param lws_protocols pointer to libwebsockets protocol structure
//------------------------------------------------------------------------------
extern WF_API void wf_server_protocol_init_lws(
    struct wf_server_protocol * protocol,
    struct lws_protocols * lws_protocol);

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
/// \param protocol pointer to protocol
/// \param type   type of the credentials the authenticator supports
/// \param authenticate function called to authenticate a user
/// \param user_data context of authenticate function
//------------------------------------------------------------------------------
extern WF_API void wf_server_protocol_add_authenticator(
    struct wf_server_protocol * protocol,
    char const * type,
    wf_authenticate_fn * authenticate,
    void * user_data);

#ifdef __cplusplus
}
#endif

#endif
