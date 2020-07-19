#ifndef WF_IMPL_JSONRPC_SEND_FN_H
#define WF_IMPL_JSONRPC_SEND_FN_H

#ifndef __cplusplus
#include <stdbool.h>
#endif


#ifdef __cplusplus
extern "C"
{
#endif

struct wf_message;

typedef bool wf_jsonrpc_send_fn(
	struct wf_message * request,
    void * user_data);

#ifdef __cplusplus
}
#endif

#endif
