////////////////////////////////////////////////////////////////////////////////
/// \file adapter/authenticate.h
/// \brief Authenticate function.
////////////////////////////////////////////////////////////////////////////////

#ifndef WF_ADAPTER_AUTHENTICATE_H
#define WF_ADAPTER_AUTHENTICATE_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_credentials;

//------------------------------------------------------------------------------
/// \brief Authentication function type.
///
/// Functions of this type are used to authenticate a user by some provided
/// credentials.
///
/// \param credentials credentials to authenticate the user
/// \param user_data   context of the authentication function
/// \return true, if authentication was successful, false otherwise
///
/// \see wf_server_config_add_authenticator
/// \see wf_server_protocol_add_authenticator
//------------------------------------------------------------------------------
typedef bool wf_authenticate_fn(
    struct wf_credentials const * credentials,
    void * user_data);

#ifdef __cplusplus
}
#endif

#endif
