#include "jsonrpc/proxy.h"
#include "jsonrpc/response.h"
#include "jsonrpc/error.h"
#include "jsonrpc/status.h"

#include <string.h>

static void jsonrpc_proxy_timeout(
    struct wf_impl_timer * timer)
{
    struct jsonrpc_proxy * proxy = timer->user_data;

    if (proxy->request.is_pending)
    {
        jsonrpc_proxy_finished_fn * finished = proxy->request.finished;
        void * user_data = proxy->request.user_data;

        proxy->request.is_pending = false;
        proxy->request.id = 0;
        proxy->request.user_data = NULL;
        proxy->request.finished = NULL;
        wf_impl_timer_cancel(&proxy->request.timer);

        jsonrpc_propate_error(finished, user_data, JSONRPC_BAD_TIMEOUT, "Timeout");
    }
}

static json_t * jsonrpc_request_create(
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

void jsonrpc_proxy_init(
    struct jsonrpc_proxy * proxy,
    struct wf_impl_timeout_manager * timeout_manager,
    int timeout,
    jsonrpc_send_fn * send,
    void * user_data)
{
    proxy->send = send;
    proxy->timeout = timeout;
    proxy->user_data = user_data;
    proxy->request.is_pending = false;
    
    wf_impl_timer_init(&proxy->request.timer, timeout_manager);
}

void jsonrpc_proxy_cleanup(
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
        wf_impl_timer_cancel(&proxy->request.timer);

        jsonrpc_propate_error(finished, user_data, JSONRPC_BAD, "Bad");
    }

    wf_impl_timer_cleanup(&proxy->request.timer);
}

void jsonrpc_proxy_invoke(
	struct jsonrpc_proxy * proxy,
	jsonrpc_proxy_finished_fn * finished,
	void * user_data,
	char const * method_name,
	char const * param_info,
	...
)
{
    if (!proxy->request.is_pending)
    {
        proxy->request.is_pending = true;
        proxy->request.finished = finished;
        proxy->request.user_data = user_data;
        proxy->request.id = 42;
        wf_impl_timer_start(&proxy->request.timer, wf_impl_timepoint_in_msec(proxy->timeout), 
                &jsonrpc_proxy_timeout, proxy);
        
        va_list args;
        va_start(args, param_info);
        json_t * request = jsonrpc_request_create(method_name, proxy->request.id, param_info, args);
        va_end(args);

        bool const is_send = ((NULL != request) && (proxy->send(request, proxy->user_data)));
        if (!is_send)
        {
            proxy->request.is_pending = false;
            proxy->request.finished = NULL;
            proxy->request.user_data = NULL;
            proxy->request.id = 0;
            wf_impl_timer_cancel(&proxy->request.timer);

            jsonrpc_propate_error(finished, user_data, JSONRPC_BAD, "Bad");
        }

        if (NULL != request)
        {
            json_decref(request);
        }
    }
    else
    {
        jsonrpc_propate_error(finished, user_data, JSONRPC_BAD_BUSY, "Busy");
    }
}

extern void jsonrpc_proxy_notify(
	struct jsonrpc_proxy * proxy,
	char const * method_name,
	char const * param_info,
	...
)
{            
    va_list args;
    va_start(args, param_info);
    json_t * request = jsonrpc_request_create(method_name, 0, param_info, args);
    va_end(args);
    if (NULL != request)
    {
        proxy->send(request, proxy->user_data);
        json_decref(request);
    }
}


void jsonrpc_proxy_onresult(
    struct jsonrpc_proxy * proxy,
    json_t * message)
{
	struct jsonrpc_response response;
	jsonrpc_response_init(&response, message);

    if ((proxy->request.is_pending) && (response.id == proxy->request.id))
    {
        jsonrpc_proxy_finished_fn * finished = proxy->request.finished;
        void * user_data = proxy->request.user_data;

        proxy->request.is_pending = false;
        proxy->request.id = 0;
        proxy->request.user_data = NULL;
        proxy->request.finished = NULL;
        wf_impl_timer_cancel(&proxy->request.timer);

        finished(user_data, response.result, response.error);
    }

    jsonrpc_response_cleanup(&response);
}

