#ifndef WF_IMPL_JSONRPC_PROXY_REQUEST_MANAGER_H
#define WF_IMPL_JSONRPC_PROXY_REQUEST_MANAGER_H

#include "webfuse/impl/jsonrpc/proxy_finished_fn.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_jsonrpc_proxy_request_manager;
struct wf_jsonrpc_response;
struct wf_timer_manager;

extern struct wf_jsonrpc_proxy_request_manager *
wf_impl_jsonrpc_proxy_request_manager_create(
    struct wf_timer_manager * timer_manager,
    int timeout);

extern void
wf_impl_jsonrpc_proxy_request_manager_dispose(
    struct wf_jsonrpc_proxy_request_manager * manager);

extern int
wf_impl_jsonrpc_proxy_request_manager_add_request(
    struct wf_jsonrpc_proxy_request_manager * manager,
    wf_jsonrpc_proxy_finished_fn * finished,
    void * user_data);

extern void
wf_impl_jsonrpc_proxy_request_manager_cancel_request(
    struct wf_jsonrpc_proxy_request_manager * manager,
    int id,
    int error_code,
    char const * error_message);

extern void
wf_impl_jsonrpc_proxy_request_manager_finish_request(
    struct wf_jsonrpc_proxy_request_manager * manager,
    struct wf_jsonrpc_response * response);


#ifdef __cplusplus
}
#endif

#endif
