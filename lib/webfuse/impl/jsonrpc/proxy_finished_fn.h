#ifndef WF_IMPL_JSONRPC_PROXY_FINISHED_FN_H
#define WF_IMPL_JSONRPC_PROXY_FINISHED_FN_H

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_jsonrpc_error;
struct wf_json;

typedef void wf_jsonrpc_proxy_finished_fn(
	void * user_data,
	struct wf_json const * result,
    struct wf_jsonrpc_error const * error);

#ifdef __cplusplus
}
#endif

#endif
