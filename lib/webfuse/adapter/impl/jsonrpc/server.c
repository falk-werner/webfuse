#include "webfuse/adapter/impl/jsonrpc/server.h"
#include <string.h>

#include "webfuse/adapter/impl/jsonrpc/method_intern.h"
#include "webfuse/adapter/impl/jsonrpc/request.h"
#include "webfuse/adapter/impl/jsonrpc/response.h"

#define WF_DEFAULT_TIMEOUT (10 * 1000)

static struct wf_impl_jsonrpc_method const * wf_impl_jsonrpc_server_getmethod(
    struct wf_impl_jsonrpc_server * server,
    char const * name)
{
    struct wf_impl_jsonrpc_method * method = server->methods;
    while ((NULL != method) && (0 == strcmp(name, method->name)))
    {
        method = method->next;
    }

    return method;
}

static void wf_impl_jsonrpc_server_timeout(
    struct wf_impl_timer * timer)
{
    struct wf_impl_jsonrpc_server * server = timer->user_data;

    if (server->request.is_pending)
    {
        wf_impl_jsonrpc_method_finished_fn * finished = server->request.finished;
        void * user_data = server->request.user_data;

        server->request.is_pending = false;
        server->request.id = 0;
        server->request.user_data = NULL;
        server->request.finished = NULL;
        wf_impl_timer_cancel(&server->request.timer);

        finished(user_data, WF_BAD_TIMEOUT, NULL);
    }
}

void wf_impl_jsonrpc_server_init(
    struct wf_impl_jsonrpc_server * server,
    struct wf_impl_timeout_manager * timeout_manager)
{
    server->methods = NULL;
    server->request.is_pending = false;
    
    wf_impl_timer_init(&server->request.timer, timeout_manager);
}

void wf_impl_jsonrpc_server_cleanup(
    struct wf_impl_jsonrpc_server * server)
{
    wf_impl_timer_cleanup(&server->request.timer);

    if (server->request.is_pending)
    {
        server->request.finished(server->request.user_data, WF_BAD, NULL);
        server->request.is_pending = false;
    }

    struct wf_impl_jsonrpc_method * method = server->methods;
    while (NULL != method)
    {
        struct wf_impl_jsonrpc_method * next = method->next;
        method->next = NULL;
        wf_impl_jsonrpc_method_dispose(method);
        method = next;
    }
    server->methods = NULL;
}

void wf_impl_jsonrpc_server_add(
    struct wf_impl_jsonrpc_server * server,
    char const * name,
    wf_impl_jsonrpc_method_invoke_fn * invoke,
    void * user_data)
{
    struct wf_impl_jsonrpc_method * method = wf_impl_jsonrpc_method_create(name, invoke, user_data);
    method->next = server->methods;
    server->methods = method;
}

void wf_impl_jsonrpc_server_invoke(
	struct wf_impl_jsonrpc_server * server,
	wf_impl_jsonrpc_method_finished_fn * finished,
	void * user_data,
	char const * method_name,
	char const * param_info,
	...
)
{
    if (!server->request.is_pending)
    {
        struct wf_impl_jsonrpc_method const * method = wf_impl_jsonrpc_server_getmethod(server, method_name);
        if (NULL != method)
        {
            server->request.is_pending = true;
            server->request.finished = finished;
            server->request.user_data = user_data;
            server->request.id = 42;
            wf_impl_timer_start(&server->request.timer, wf_impl_timepoint_in_msec(WF_DEFAULT_TIMEOUT), 
                    &wf_impl_jsonrpc_server_timeout, server);
            
            va_list args;
            va_start(args, param_info);
            json_t * request = wf_impl_jsonrpc_request_create(method_name, server->request.id, param_info, args);
            va_end(args);
            if (NULL != request)
            {
                if (!method->invoke(method->user_data, request))
                {
                    server->request.is_pending = false;
                    server->request.finished = NULL;
                    server->request.user_data = NULL;
                    server->request.id = 0;
                    wf_impl_timer_cancel(&server->request.timer);

                    finished(user_data, WF_BAD, NULL);
                }
                json_decref(request);
            }
        }
        else
        {
            finished(user_data, WF_BAD_NOTIMPLEMENTED, NULL);
        }        
    }
    else
    {
        finished(user_data, WF_BAD_BUSY, NULL);
    }
}

extern void wf_impl_jsonrpc_server_notify(
	struct wf_impl_jsonrpc_server * server,
	char const * method_name,
	char const * param_info,
	...
)
{
        struct wf_impl_jsonrpc_method const * method = wf_impl_jsonrpc_server_getmethod(server, method_name);
        if (NULL != method)
        {
            
            va_list args;
            va_start(args, param_info);
            json_t * request = wf_impl_jsonrpc_request_create(method_name, 0, param_info, args);
            va_end(args);
            if (NULL != request)
            {
                method->invoke(method->user_data, request);
                json_decref(request);
            }
        }

}


void wf_impl_jsonrpc_server_onresult(
    struct wf_impl_jsonrpc_server * server,
    char const * message,
    size_t length)
{
	struct wf_impl_jsonrpc_response response;
	wf_impl_jsonrpc_response_init(&response, message, length);

    if ((server->request.is_pending) && (response.id == server->request.id))
    {
        wf_impl_jsonrpc_method_finished_fn * finished = server->request.finished;
        void * user_data = server->request.user_data;

        server->request.is_pending = false;
        server->request.id = 0;
        server->request.user_data = NULL;
        server->request.finished = NULL;
        wf_impl_timer_cancel(&server->request.timer);

        finished(user_data, response.status, response.result);
    }

    wf_impl_jsonrpc_response_cleanup(&response);
}

