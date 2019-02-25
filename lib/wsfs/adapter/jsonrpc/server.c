#include "wsfs/adapter/jsonrpc/server.h"
#include <string.h>

#include "wsfs/adapter/jsonrpc/method_intern.h"
#include "wsfs/adapter/jsonrpc/request.h"
#include "wsfs/adapter/jsonrpc/response.h"

#define WSFS_DEFAULT_TIMEOUT (10 * 1000)

static struct wsfs_jsonrpc_method const * wsfs_jsonrpc_server_getmethod(
    struct wsfs_jsonrpc_server * server,
    char const * name)
{
    struct wsfs_jsonrpc_method * method = server->methods;
    while ((NULL != method) && (0 == strcmp(name, method->name)))
    {
        method = method->next;
    }

    return method;
}

static void wsfs_jsonrpc_server_timeout(
    struct wsfs_timer * timer)
{
    struct wsfs_jsonrpc_server * server = timer->user_data;

    if (server->request.is_pending)
    {
        wsfs_jsonrpc_method_finished_fn * finished = server->request.finished;
        void * user_data = server->request.user_data;

        server->request.is_pending = false;
        server->request.id = 0;
        server->request.user_data = NULL;
        server->request.finished = NULL;
        wsfs_timer_cancel(&server->request.timer);

        finished(user_data, WSFS_BAD_TIMEOUT, NULL);
    }
}

void wsfs_jsonrpc_server_init(
    struct wsfs_jsonrpc_server * server,
    struct wsfs_timeout_manager * timeout_manager)
{
    server->methods = NULL;
    server->request.is_pending = false;
    
    wsfs_timer_init(&server->request.timer, timeout_manager);
}

void wsfs_jsonrpc_server_cleanup(
    struct wsfs_jsonrpc_server * server)
{
    wsfs_timer_cleanup(&server->request.timer);

    if (server->request.is_pending)
    {
        server->request.finished(server->request.user_data, WSFS_BAD, NULL);
        server->request.is_pending = false;
    }

    struct wsfs_jsonrpc_method * method = server->methods;
    while (NULL != method)
    {
        struct wsfs_jsonrpc_method * next = method->next;
        method->next = NULL;
        wsfs_jsonrpc_method_dispose(method);
        method = next;
    }
    server->methods = NULL;
}

void wsfs_jsonrpc_server_add(
    struct wsfs_jsonrpc_server * server,
    char const * name,
    wsfs_jsonrpc_method_invoke_fn * invoke,
    void * user_data)
{
    struct wsfs_jsonrpc_method * method = wsfs_jsonrpc_method_create(name, invoke, user_data);
    method->next = server->methods;
    server->methods = method;
}

void wsfs_jsonrpc_server_invoke(
	struct wsfs_jsonrpc_server * server,
	wsfs_jsonrpc_method_finished_fn * finished,
	void * user_data,
	char const * method_name,
	char const * param_info,
	...
)
{
    if (!server->request.is_pending)
    {
        struct wsfs_jsonrpc_method const * method = wsfs_jsonrpc_server_getmethod(server, method_name);
        if (NULL != method)
        {
            server->request.is_pending = true;
            server->request.finished = finished;
            server->request.user_data = user_data;
            server->request.id = 42;
            wsfs_timer_start(&server->request.timer, wsfs_timepoint_in_msec(WSFS_DEFAULT_TIMEOUT), 
                    &wsfs_jsonrpc_server_timeout, server);
            
            va_list args;
            va_start(args, param_info);
            json_t * request = wsfs_jsonrpc_request_create(method_name, server->request.id, param_info, args);
            va_end(args);
            if (NULL != request)
            {
                if (!method->invoke(method->user_data, request))
                {
                    server->request.is_pending = false;
                    server->request.finished = NULL;
                    server->request.user_data = NULL;
                    server->request.id = 0;
                    wsfs_timer_cancel(&server->request.timer);

                    finished(user_data, WSFS_BAD, NULL);
                }
                json_decref(request);
            }
        }
        else
        {
            finished(user_data, WSFS_BAD_NOTIMPLEMENTED, NULL);
        }        
    }
    else
    {
        finished(user_data, WSFS_BAD_BUSY, NULL);
    }
}

extern void wsfs_jsonrpc_server_notify(
	struct wsfs_jsonrpc_server * server,
	char const * method_name,
	char const * param_info,
	...
)
{
        struct wsfs_jsonrpc_method const * method = wsfs_jsonrpc_server_getmethod(server, method_name);
        if (NULL != method)
        {
            
            va_list args;
            va_start(args, param_info);
            json_t * request = wsfs_jsonrpc_request_create(method_name, 0, param_info, args);
            va_end(args);
            if (NULL != request)
            {
                method->invoke(method->user_data, request);
                json_decref(request);
            }
        }

}


void wsfs_jsonrpc_server_onresult(
    struct wsfs_jsonrpc_server * server,
    char const * message,
    size_t length)
{
	struct wsfs_jsonrpc_response response;
	wsfs_jsonrpc_response_init(&response, message, length);

    if ((server->request.is_pending) && (response.id == server->request.id))
    {
        wsfs_jsonrpc_method_finished_fn * finished = server->request.finished;
        void * user_data = server->request.user_data;

        server->request.is_pending = false;
        server->request.id = 0;
        server->request.user_data = NULL;
        server->request.finished = NULL;
        wsfs_timer_cancel(&server->request.timer);

        finished(user_data, response.status, response.result);
    }

    wsfs_jsonrpc_response_cleanup(&response);
}

