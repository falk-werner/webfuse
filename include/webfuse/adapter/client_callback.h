////////////////////////////////////////////////////////////////////////////////
/// \file adapter/client_callbak.h
/// \brief Callback of adapter clients.
////////////////////////////////////////////////////////////////////////////////

#ifndef WF_ADAPTER_CLIENT_CALLBACK_H
#define WF_ADAPTER_CLIENT_CALLBACK_H

#ifdef __cplusplus
extern "C"
{
#endif


#define WF_CLIENT_INIT                         0x0001   ///< Out-most initialization of a client
#define WF_CLIENT_CLEANUP                      0x0002   ///< Out-most cleanup of a client
#define WF_CLIENT_CREATED                      0x0003   ///< Client is fully initialized an can be used

#define WF_CLIENT_CONNECTED                    0x0011   ///< Connection to a foreign provider established
#define WF_CLIENT_DISCONNECTED                 0x0012   ///< Connection closed or connect failed

#define WF_CLIENT_AUTHENTICATED                0x0021   ///< Authentication succeeded
#define WF_CLIENT_AUTHENTICATION_FAILED        0x0022   ///< Authentication failed
#define WF_CLIENT_AUTHENTICATE_GET_CREDENTIALS 0x0023   ///< Query credentials (\see wf_client_authenticate)

#define WF_CLIENT_FILESYSTEM_ADDED             0x0031   ///< File system added successfully
#define WF_CLIENT_FILESYSTEM_ADD_FAILED        0x0032   ///< Failed to add file system

#define WF_CLIENT_GET_TLS_CONFIG               0x0041   ///< Query TLS config (\see wf_client_create)

struct wf_client;

//------------------------------------------------------------------------------
/// \brief Client callback function
///
/// This function is triggered whenever an event occurs the client should
/// be aware of.
///
/// \param client Pointer to the client
/// \param reason Event, that triggered the callback
/// \param args   Event-specific arguments
//------------------------------------------------------------------------------
typedef void wf_client_callback_fn(
    struct wf_client * client,
    int reason,
    void * args);

#ifdef __cplusplus
}
#endif


#endif
