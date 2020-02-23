#ifndef WF_LWS_UTILS_H
#define WF_LWS_UTILS_H

#include <libwebsockets.h>

#ifdef __cplusplus
extern "C"
{
#endif

extern char const * 
wf_lws_reason_tostring(
    enum lws_callback_reasons reason);

#ifdef __cplusplus
}
#endif

#endif
