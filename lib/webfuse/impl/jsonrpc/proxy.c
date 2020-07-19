#include "webfuse/impl/jsonrpc/proxy_intern.h"
#include "webfuse/impl/jsonrpc/proxy_request_manager.h"
#include "webfuse/impl/jsonrpc/response_intern.h"
#include "webfuse/impl/jsonrpc/error.h"
#include "webfuse/impl/json/writer.h"
#include "webfuse/impl/message.h"
#include "webfuse/status.h"

#include <libwebsockets.h>

#include <stdlib.h>
#include <string.h>

#define WF_JSONRPC_PROXY_DEFAULT_MESSAGE_SIZE 1024

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


static struct wf_message * 
wf_impl_jsonrpc_request_create(
	char const * method,
	int id,
	char const * param_info,
	va_list args)
{
    struct wf_json_writer * writer = wf_impl_json_writer_create(WF_JSONRPC_PROXY_DEFAULT_MESSAGE_SIZE, LWS_PRE);
    wf_impl_json_write_object_begin(writer);
    wf_impl_json_write_object_string(writer, "method", method);
    wf_impl_json_write_object_begin_array(writer, "params");
	
	for (char const * param_type = param_info; '\0' != *param_type; param_type++)
	{
		switch(*param_type)
		{
			case 's':
			{
				char const * const value = va_arg(args, char const *);
                wf_impl_json_write_string(writer, value);
			}
			break;
			case 'i':
			{
				int const value = va_arg(args, int);
                wf_impl_json_write_int(writer, value);
			}
			break;
            case 'j':
            {
                wf_jsonrpc_custom_write_fn * write = va_arg(args, wf_jsonrpc_custom_write_fn *);
                void * data = va_arg(args, void *);
                write(writer, data);
            }
            break;
			default:
			    fprintf(stderr, "fatal: unknown param_type '%c'\n", *param_type);
            break;
		}
	}
	wf_impl_json_write_array_end(writer);

	if (0 != id)
	{
		wf_impl_json_write_object_int(writer, "id", id);
	}
    
    wf_impl_json_write_object_end(writer);
	
    size_t length;
    char * message = wf_impl_json_writer_take(writer, &length);
    wf_impl_json_writer_dispose(writer);

	return wf_impl_message_create(message, length);
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

    struct wf_message * request = wf_impl_jsonrpc_request_create(method_name, id, param_info, args);
    bool const is_send = proxy->send(request, proxy->user_data);
    if (!is_send)
    {
        wf_impl_jsonrpc_proxy_request_manager_cancel_request(
            proxy->request_manager, id, WF_BAD, "Bad: failed to send request");
    }
}

extern void wf_impl_jsonrpc_proxy_vnotify(
	struct wf_jsonrpc_proxy * proxy,
	char const * method_name,
	char const * param_info,
	va_list args)
{
    struct wf_message * request = wf_impl_jsonrpc_request_create(method_name, 0, param_info, args);
    proxy->send(request, proxy->user_data);
}


void wf_impl_jsonrpc_proxy_onresult(
    struct wf_jsonrpc_proxy * proxy,
    struct wf_json const * message)
{
	struct wf_jsonrpc_response response;
	wf_impl_jsonrpc_response_init(&response, message);

    wf_impl_jsonrpc_proxy_request_manager_finish_request(
        proxy->request_manager, &response);

    wf_impl_jsonrpc_response_cleanup(&response);
}
