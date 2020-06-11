#ifndef WF_ADAPTER_CLIENT_CALLBACK_H
#define WF_ADAPTER_CLIENT_CALLBACK_H

#ifdef __cplusplus
extern "C"
{
#endif


#define WF_CLIENT_CREATED                      0x0001
#define WF_CLIENT_DISPOSING                    0x0002

#define WF_CLIENT_CONNECTED                    0x0011
#define WF_CLIENT_DISCONNECTED                 0x0012

#define WF_CLIENT_AUTHENTICATED                0x0021
#define WF_CLIENT_AUTHENTICATION_FAILED        0x0022
#define WF_CLIENT_AUTHENTICATE_GET_CREDENTIALS 0x0023

#define WF_CLIENT_FILESYSTEM_ADDED             0x0031
#define WF_CLIENT_FILESYSTEM_ADD_FAILED        0x0032

struct wf_client;

typedef void wf_client_callback_fn(
    struct wf_client * client,
    int reason,
    void * args);

#ifdef __cplusplus
}
#endif


#endif
