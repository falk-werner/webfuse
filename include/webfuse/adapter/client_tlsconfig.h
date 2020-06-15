////////////////////////////////////////////////////////////////////////////////
/// \file adapter/client_tslconfig.h
/// \brief Configuration of TLS (Transport Layer Security) for adapter clients.
////////////////////////////////////////////////////////////////////////////////

#ifndef WF_ADAPTER_CLIENT_TLSCONFIG_H
#define WF_ADAPTER_CLIENT_TLSCONFIG_H

#include <webfuse/adapter/api.h>

#ifdef __cplusplus
extern "C"
{
#endif

//------------------------------------------------------------------------------
/// \struct wf_client_tlsconfig
/// \brief TLS configuration of the client.
///
/// TLS configuration is queried during initialization of a client via
/// WF_CLIENT_GET_TLS_CONFIG event.
///
/// \see WF_CLIENT_GET_TLS_CONFIG
/// \see wf_client_create
//------------------------------------------------------------------------------
struct wf_client_tlsconfig;

//------------------------------------------------------------------------------
/// \brief Sets the path to the private key.
///
/// \note To enable TLS both, key_path and cert_path, must be specified.
///
/// \param config Pointer to config.
/// \param key_path Path to private key file (in PEM format).
//------------------------------------------------------------------------------
extern WF_API void
wf_client_tlsconfig_set_keypath(
    struct wf_client_tlsconfig * config,
    char const * key_path);

//------------------------------------------------------------------------------
/// \brief Sets the path to the clients certificate.
///
/// \note To enable TLS both, key_path and cert_path, must be specified.
///
/// \param config Pointer to the config.
/// \param cert_path Path the the clients certificate (in PEM format).
//------------------------------------------------------------------------------
extern WF_API void
wf_client_tlsconfig_set_certpath(
    struct wf_client_tlsconfig * config,
    char const * cert_path);

//------------------------------------------------------------------------------
/// \brief Sets the path the CA file.
///
/// \param config Pointer to the config.
/// \param cafile_path Path to CA file (in PEM format).
//------------------------------------------------------------------------------
extern WF_API void
wf_client_tlsconfig_set_cafilepath(
    struct wf_client_tlsconfig * config,
    char const * cafile_path);

#ifdef __cplusplus
}
#endif

#endif
