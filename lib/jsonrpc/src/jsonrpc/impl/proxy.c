#include "jsonrpc/impl/proxy.h"
#include "jsonrpc/impl/response.h"
#include "jsonrpc/impl/error.h"
#include "jsonrpc/status.h"

#include <wf/timer/timer.h>

#include <stdlib.h>
#include <string.h>

struct jsonrpc_proxy *
jsonrpc_impl_proxy_create(
    struct wf_timer_manager * manager,
    int timeout,
    jsonrpc_send_fn * send,
    void * user_data)
{
    struct jsonrpc_proxy * proxy = malloc(sizeof(struct jsonrpc_proxy));
    if (NULL != proxy)
    {
        jsonrpc_impl_proxy_init(proxy, manager, timeout, send, user_data);
    }

    return proxy;
}

void jsonrpc_impl_proxy_dispose(
    struct jsonrpc_proxy * proxy)
{
    jsonrpc_impl_proxy_cleanup(proxy);
    free(proxy);
}

static void jsonrpc_impl_proxy_on_timeout(
    struct wf_timer * timer, void * proxy_ptr)
{
    struct jsonrpc_proxy * proxy = proxy_ptr;

    if (proxy->request.is_pending)
    {
        jsonrpc_proxy_finished_fn * finished = proxy->request.finished;
        void * user_data = proxy->request.user_data;

        proxy->request.is_pending = false;
        proxy->request.id = 0;
        proxy->request.user_data = NULL;
        proxy->request.finished = NULL;
        wf_timer_cancel(timer);

        jsonrpc_impl_propate_error(finished, user_data, JSONRPC_BAD_TIMEOUT, "Timeout");
    }
}

static json_t * jsonrpc_impl_request_create(
	char const * method,
	int id,
	char const * param_info,
	va_list args)
{
	json_t * request = json_object();
	json_object_set_new(request, "method", json_string(method));
	json_t * params = json_array();
	
	for (char const * param_type = param_info; '\0' != *param_type; param_type++)
	{
		switch(*param_type)
		{
			case 's':
			{
				char const * const value = va_arg(args, char const *);
				json_array_append_new(params, json_string(value));
			}
			break;
			case 'i':
			{
				int const value = va_arg(args, int);
				json_array_append_new(params, json_integer(value));
			}
			break;
			default:
			fprintf(stderr, "fatal: unknown param_type '%c'\n", *param_type);
            json_decref(params);
            json_decref(request);
            return NULL;
		}
	}
	

	json_object_set_new(request, "params", params);
	if (0 != id)
	{
		json_object_set_new(request, "id", json_integer(id));
	}
	
	return request;
}

void jsonrpc_impl_proxy_init(
    struct jsonrpc_proxy * proxy,
    struct wf_timer_manager * timeout_manager,
    int timeout,
    jsonrpc_send_fn * send,
    void * user_data)
{
    proxy->send = send;
    proxy->timeout = timeout;
    proxy->user_data = user_data;
    proxy->request.is_pending = false;
    proxy->request.timer = wf_timer_create(timeout_manager, 
        &jsonrpc_impl_proxy_on_timeout, proxy);    
}

void jsonrpc_impl_proxy_cleanup(
    struct jsonrpc_proxy * proxy)
{
    if (proxy->request.is_pending)
    {
        void * user_data = proxy->request.user_data;
        jsonrpc_proxy_finished_fn * finished = proxy->request.finished;

        proxy->request.is_pending = false;
        proxy->request.finished = NULL;
        proxy->request.user_data = NULL;
        proxy->request.id = 0;
        wf_timer_cancel(proxy->request.timer);

        jsonrpc_impl_propate_error(finished, user_data, JSONRPC_BAD, "Bad");
    }

    wf_timer_dispose(proxy->request.timer);
}

void jsonrpc_impl_proxy_invoke(
	struct jsonrpc_proxy * proxy,
	jsonrpc_proxy_finished_fn * finished,
	void * user_data,
	char const * method_name,
	char const * param_info,
	va_list args
)
{
    if (!proxy->request.is_pending)
    {
        proxy->request.is_pending = true;
        proxy->request.finished = finished;
        proxy->request.user_data = user_data;
        proxy->request.id = 42;
        wf_timer_start(proxy->request.timer, proxy->timeout);
        
        json_t * request = jsonrpc_impl_request_create(method_name, proxy->request.id, param_info, args);

        bool const is_send = ((NULL != request) && (proxy->send(request, proxy->user_data)));
        if (!is_send)
        {
            proxy->request.is_pending = false;
            proxy->request.finished = NULL;
            proxy->request.user_data = NULL;
            proxy->request.id = 0;
            wf_timer_cancel(proxy->request.timer);

            jsonrpc_impl_propate_error(finished, user_data, JSONRPC_BAD, "Bad");
        }

        if (NULL != request)
        {
            json_decref(request);
        }
    }
    else
    {
        jsonrpc_impl_propate_error(finished, user_data, JSONRPC_BAD_BUSY, "Busy");
    }
}

extern void jsonrpc_impl_proxy_notify(
	struct jsonrpc_proxy * proxy,
	char const * method_name,
	char const * param_info,
	va_list args
)
{            
    json_t * request = jsonrpc_impl_request_create(method_name, 0, param_info, args);
    if (NULL != request)
    {
        proxy->send(request, proxy->user_data);
        json_decref(request);
    }
}


void jsonrpc_impl_proxy_onresult(
    struct jsonrpc_proxy * proxy,
    json_t * message)
{
	struct jsonrpc_response response;
	jsonrpc_impl_response_init(&response, message);

    if ((proxy->request.is_pending) && (response.id == proxy->request.id))
    {
        jsonrpc_proxy_finished_fn * finished = proxy->request.finished;
        void * user_data = proxy->request.user_data;

        proxy->request.is_pending = false;
        proxy->request.id = 0;
        proxy->request.user_data = NULL;
        proxy->request.finished = NULL;
        wf_timer_cancel(proxy->request.timer);

        finished(user_data, response.result, response.error);
    }

    jsonrpc_impl_response_cleanup(&response);
}

