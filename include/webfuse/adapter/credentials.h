////////////////////////////////////////////////////////////////////////////////
/// \file adapter/credentials.h
/// \brief Credentials used for user authentication.
////////////////////////////////////////////////////////////////////////////////

#ifndef WF_ADAPTER_CREDENTIALS_H
#define WF_ADAPTER_CREDENTIALS_H

#include "webfuse/adapter/api.h"

#ifdef __cplusplus
extern "C"
{
#endif

//------------------------------------------------------------------------------
/// \struct wf_credentials
/// \brief Credentials used for user authentication.
///
/// Credentials are used during authentication to authenticate a user.
/// In order to support multiple types of credentials, e.g. username + password,
/// certifactes and / or authentication tokens, credentials are modelled as
/// opaque type with some access functions.
///
/// \see wf_authenticate_fn
/// \see wf_credentials_type
/// \see wf_credentials_get
//------------------------------------------------------------------------------
struct wf_credentials;

//------------------------------------------------------------------------------
/// \brief Returns the type of the credentials object.
///
/// The type of the credentials objects defines, which keys are available
/// for a given instance.
///
/// \note When an authenticate function is called, the credentials type
///       matches the type provided during _add_authenticator function call.
///       Therefore, it is not necessary to check credentials type within
///       the authenticate function.
///
/// \note Within webfuse protocol documentation, only one type of credentials 
///       is described byte now: username
///
/// \param credentials Pointer to credentials object
/// \return type of the credentials object
///
/// \see wf_server_config_add_authenticator
/// \see wf_server_protocol_add_authenticator
/// \see wf_authenticate_fn
//------------------------------------------------------------------------------
extern WF_API char const * wf_credentials_type(
    struct wf_credentials const * credentials);

//------------------------------------------------------------------------------
/// \brief Return the value of a credentials item identified by the provided
///        key.
///
/// \param credentials Pointer to credentials object.
/// \param key String to identify the item.
/// \return value of credentials item or null, if there is no item with that
///         key
//------------------------------------------------------------------------------
extern WF_API char const * wf_credentials_get(
    struct wf_credentials const * credentials,
    char const * key);

//------------------------------------------------------------------------------
/// \brief Sets the type of the credentials.
///
/// \param credentials Pointer to credentials object.
/// \param type Type of credentials.
//------------------------------------------------------------------------------
extern WF_API void wf_credentials_set_type(
    struct wf_credentials * credentials,
    char const * type);

//------------------------------------------------------------------------------
/// \brief Adds an item to credentials
///
/// \param credentials Pointer to credentials object.
/// \param key String to identify the item.
/// \param key Value of the item.
//------------------------------------------------------------------------------
extern WF_API void wf_credentials_add(
    struct wf_credentials * credentials,
    char const * key,
    char const * value);

#ifdef __cplusplus
}
#endif


#endif
