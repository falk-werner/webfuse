////////////////////////////////////////////////////////////////////////////////
/// \file adapter/client.h
/// \brief Adapter client.
////////////////////////////////////////////////////////////////////////////////

#ifndef WF_ADAPTER_CLIENT_H
#define WF_ADAPTER_CLIENT_H

#include <webfuse/adapter/api.h>
#include <webfuse/adapter/client_callback.h>

#ifdef __cplusplus
extern "C"
{
#endif

//------------------------------------------------------------------------------
/// \struct wf_client
/// \brief Adapter client
///
/// An adapter client is used to connect to a remote provider server, e.g. a
/// cloud service, an request a filesystem to inject.
//------------------------------------------------------------------------------
struct wf_client;

//------------------------------------------------------------------------------
/// \brief Created a new instance of an adapter client.
///
/// During creation, the client is initialized in the following order:
/// - WF_CLIENT_INIT is triggered to initialize custom data
/// - WF_CLIENT_GET_TLS_CONFIG is triggered to query TLS configuration 
/// - internal initialization is performed
/// - WF_CLIENT_CREATED is triggered
///
/// Therefore, the client should not be used, unless WF_CLIENT_CREATED is
/// triggered.
///
/// When TLS configuration is queried, a pointer to an instance if 
/// \see wf_client_tlsconfig is provided to be set by the user.
///
/// \param callback  Pointer to the callback function.
/// \param user_data Pointer to user data.
///
/// \return Newly created instance of an adapter client.
//------------------------------------------------------------------------------
extern WF_API struct wf_client *
wf_client_create(
    wf_client_callback_fn * callback,
    void * user_data);

//------------------------------------------------------------------------------
/// \brief Disposes an adapter client.
///
/// \param client Pointer to the client.
//------------------------------------------------------------------------------
extern WF_API void
wf_client_dispose(
    struct wf_client * client);

//------------------------------------------------------------------------------
/// \brief Get user data.
///
/// \param client Pointer to the client.
//------------------------------------------------------------------------------
extern WF_API void *
wf_client_get_userdata(
    struct wf_client * client);

//------------------------------------------------------------------------------
/// \brief Triggers the client.
///
/// \param client Pointer to the client.
//------------------------------------------------------------------------------
extern WF_API void
wf_client_service(
    struct wf_client * client);

//------------------------------------------------------------------------------
/// \brief Interrupts a service call.
///
/// Wakes up the client.
///
/// \note This is the only function that can be used safely from another 
// thread.
///
/// \param client Pointer to the client.
//------------------------------------------------------------------------------
extern WF_API void
wf_client_interrupt(
    struct wf_client * client);

//------------------------------------------------------------------------------
/// \brief Connects to a foreign server.
///
/// Starts to connect. The callback is triggered, when connect is finised:
/// - WF_CLIENT_CONNECTED on success
/// - WF_CLIENT_DISCONNECTED on connect error
///
/// \param client Pointer to the client.
/// \param url URL of the remote ppovider.
//------------------------------------------------------------------------------
extern WF_API void
wf_client_connect(
    struct wf_client * client,
    char const * url);

//------------------------------------------------------------------------------
/// \brief Disconnects from the foreign server.
///
/// The event WF_CLIENT_DISCONNECTED is triggered, when disconnected.
///
/// \param client Pointer to the client.
//------------------------------------------------------------------------------
extern WF_API void
wf_client_disconnect(
    struct wf_client * client);

//------------------------------------------------------------------------------
/// \brief Authenticated the client.
///
/// During authentication, credentials are queries via
/// WF_CLIENT_AUTHENTICATE_GET_CREDENTIALS event. In that case a pointer to an
/// instance of wf_client_tlsconfig is provided to set credentials.
///
/// When authentications finishes, an event is triggered:
/// - WF_CLIENT_AUTHENTICATED on success
/// - WF_CLIENT_AUTHENTICATION_FAILED on failure
///
/// \param client Pointer to the client.
//------------------------------------------------------------------------------
extern WF_API void
wf_client_authenticate(
    struct wf_client * client);

//------------------------------------------------------------------------------
/// \brief Add a filesystem.
///
/// After add filesystem finished, an event is triggered:
/// - WF_CLIENT_FILESYSTEM_ADDED on success
/// - WF_CLIENT_FILESYSTEM_ADD_FAILED on failure
///
/// \note Currently, only one file system is supported by the client.
///
/// \param client Pointer to the client.
/// \param local_path Local path where the filesystem should be places
///                   (must be an exististing and empty directory).
/// \param name Name of the filesystem (identifier sent to the provider).
//------------------------------------------------------------------------------
extern WF_API void
wf_client_add_filesystem(
    struct wf_client * client,
    char const * local_path,
    char const * name);

#ifdef __cplusplus
}
#endif

#endif