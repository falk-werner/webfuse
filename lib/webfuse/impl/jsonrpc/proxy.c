#include "webfuse/impl/jsonrpc/proxy_intern.h"
#include "webfuse/impl/jsonrpc/proxy_request_manager.h"
#include "webfuse/impl/jsonrpc/response_intern.h"
#include "webfuse/impl/jsonrpc/error.h"
#include "webfuse/status.h"


#include <stdlib.h>
#include <string.h>

struct wf_jsonrpc_proxy *
wf_impl_jsonrpc_proxy_create(
    struct wf_timer_manager * manager,
    int timeout,
    wf_jsonrpc_send_fn * send,
    void * user_data)
{
    struct wf_jsonrpc_proxy * proxy = malloc(sizeof(struct wf_jsonrpc_proxy));
    wf_impl_jsonrpc_proxy_init(proxy, manager, timeout, send, user_data);

    return proxy;
}

void wf_impl_jsonrpc_proxy_dispose(
    struct wf_jsonrpc_proxy * proxy)
{
    wf_impl_jsonrpc_proxy_cleanup(proxy);
    free(proxy);
}


static json_t * wf_impl_jsonrpc_request_create(
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
            case 'j':
            {
                json_t * const value = va_arg(args, json_t *);
                json_array_append_new(params, value);
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

void wf_impl_jsonrpc_proxy_init(
    struct wf_jsonrpc_proxy * proxy,
    struct wf_timer_manager * timeout_manager,
    int timeout,
    wf_jsonrpc_send_fn * send,
    void * user_data)
{
    proxy->send = send;
    proxy->user_data = user_data;

    proxy->request_manager = wf_impl_jsonrpc_proxy_request_manager_create(
        timeout_manager, timeout);
}

void wf_impl_jsonrpc_proxy_cleanup(
    struct wf_jsonrpc_proxy * proxy)
{
    wf_impl_jsonrpc_proxy_request_manager_dispose(proxy->request_manager);
}

void wf_impl_jsonrpc_proxy_vinvoke(
	struct wf_jsonrpc_proxy * proxy,
	wf_jsonrpc_proxy_finished_fn * finished,
	void * user_data,
	char const * method_name,
	char const * param_info,
	va_list args)
{
    int id = wf_impl_jsonrpc_proxy_request_manager_add_request(
            proxy->request_manager, finished, user_data);

    json_t * request = wf_impl_jsonrpc_request_create(method_name, id, param_info, args);
    bool const is_send = ((NULL != request) && (proxy->send(request, proxy->user_data)));
    if (!is_send)
    {
        wf_impl_jsonrpc_proxy_request_manager_cancel_request(
            proxy->request_manager, id, WF_BAD, "Bad: failed to send request");
    }

    if (NULL != request)
    {
        json_decref(request);
    }
}

extern void wf_impl_jsonrpc_proxy_vnotify(
	struct wf_jsonrpc_proxy * proxy,
	char const * method_name,
	char const * param_info,
	va_list args)
{
    json_t * request = wf_impl_jsonrpc_request_create(method_name, 0, param_info, args);

    if (NULL != request)
    {
        proxy->send(request, proxy->user_data);
        json_decref(request);
    }
}


void wf_impl_jsonrpc_proxy_onresult(
    struct wf_jsonrpc_proxy * proxy,
    json_t * message)
{
	struct wf_jsonrpc_response response;
	wf_impl_jsonrpc_response_init(&response, message);

    wf_impl_jsonrpc_proxy_request_manager_finish_request(
        proxy->request_manager, &response);

    wf_impl_jsonrpc_response_cleanup(&response);
}
