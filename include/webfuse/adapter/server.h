////////////////////////////////////////////////////////////////////////////////
/// \file adapter/server.h
/// \brief Adapter server.
////////////////////////////////////////////////////////////////////////////////

#ifndef WF_ADAPTER_SERVER_H
#define WF_ADAPTER_SERVER_H

#include "webfuse/adapter/api.h"

#ifdef __cplusplus
extern "C"
{
#endif

//------------------------------------------------------------------------------
/// \struct wf_server
/// \brief Webfuse adapter server object.
//------------------------------------------------------------------------------
struct wf_server;

struct wf_server_config;

//------------------------------------------------------------------------------
/// \brief Creates a new server.
///
/// \param config pointer to server configuration.
/// \return newly created server or NULL in case of an error
//------------------------------------------------------------------------------
extern WF_API struct wf_server * wf_server_create(
    struct wf_server_config * config);

//------------------------------------------------------------------------------
/// \brief Disposes a server.
///
/// \note server configuration is not managed by server.
///
/// \param server pointer to server
//------------------------------------------------------------------------------
extern WF_API void wf_server_dispose(
    struct wf_server * server);

//------------------------------------------------------------------------------
/// \brief Triggers the server.
///
/// This function must be invoked in a loop while the server is running. It
/// makes the server wait for the next event and processes it.
///
/// \param server pointer to server
/// \param timeout_ms timeout in milliseconds.
//------------------------------------------------------------------------------
extern WF_API void wf_server_service(
    struct wf_server * server,
    int timeout_ms);

#ifdef __cplusplus
}
#endif


#endif
