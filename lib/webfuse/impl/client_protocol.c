#include "webfuse/impl/client_protocol.h"
#include "webfuse/client_callback.h"
#include "webfuse/impl/credentials.h"
#include "webfuse/impl/filesystem.h"
#include "webfuse/impl/mountpoint.h"
#include "webfuse/protocol_names.h"
#include "webfuse/impl/util/url.h"
#include "webfuse/impl/util/util.h"
#include "webfuse/impl/timer/manager.h"
#include "webfuse/impl/jsonrpc/response.h"
#include "webfuse/impl/jsonrpc/proxy.h"

#include "webfuse/impl/message.h"
#include "webfuse/impl/message_queue.h"
#include "webfuse/impl/util/container_of.h"


#include <stddef.h>
#include <libwebsockets.h>

#define WF_DEFAULT_TIMEOUT (10 * 1000)
#define WF_DEFAULT_MESSAGE_SIZE (10 * 1024)

struct wf_impl_client_protocol_add_filesystem_context
{
    struct wf_client_protocol * protocol;
    char * local_path;
};

static void
wf_impl_client_protocol_process(
     struct wf_client_protocol * protocol, 
     char const * data,
     size_t length)
{

    json_t * message = json_loadb(data, length, 0, NULL);
    if (NULL != message)
    {
        if (wf_impl_jsonrpc_is_response(message))
        {
            wf_impl_jsonrpc_proxy_onresult(protocol->proxy, message);
        }

        json_decref(message);
    }
}

static void
wf_impl_client_protocol_receive(
     struct wf_client_protocol * protocol, 
     char const * data,
     size_t length,
     bool is_final_fragment)
{
    if (is_final_fragment)
    {
        if (wf_impl_buffer_is_empty(&protocol->recv_buffer))
        {
            wf_impl_client_protocol_process(protocol, data, length);
        }
        else
        {
            wf_impl_buffer_append(&protocol->recv_buffer, data, length);
            wf_impl_client_protocol_process(protocol,
                wf_impl_buffer_data(&protocol->recv_buffer),
                wf_impl_buffer_size(&protocol->recv_buffer));
            wf_impl_buffer_clear(&protocol->recv_buffer);
        }        
    }
    else
    {
        wf_impl_buffer_append(&protocol->recv_buffer, data, length);
    }
}


static bool
wf_impl_client_protocol_send(
    struct wf_message * message,
    void * user_data)
{
    bool result = false;
    struct wf_client_protocol * protocol = user_data;

    if (NULL != protocol->wsi)
    {
        wf_impl_slist_append(&protocol->messages, &message->item);
        lws_callback_on_writable(protocol->wsi);
        result = true;
    }
    else
    {
        wf_impl_message_dispose(message);
    }

    return result;
}


static void
wf_impl_client_protocol_on_authenticate_finished(
	void * user_data,
	json_t const * result,
	struct wf_jsonrpc_error const * WF_UNUSED_PARAM(error))    
{
    struct wf_client_protocol * protocol = user_data;
    int const reason = (NULL != result) ? WF_CLIENT_AUTHENTICATED : WF_CLIENT_AUTHENTICATION_FAILED;

    protocol->callback(protocol->user_data, reason, NULL);
}

static void
wf_impl_client_protocol_on_add_filesystem_finished(
	void * user_data,
	json_t const * result,
	struct wf_jsonrpc_error const * WF_UNUSED_PARAM(error))
{
    struct wf_impl_client_protocol_add_filesystem_context * context = user_data;
    struct wf_client_protocol * protocol = context->protocol;

    int reason = WF_CLIENT_FILESYSTEM_ADD_FAILED;
    if (NULL == protocol->filesystem)
    {
        json_t * id = json_object_get(result, "id");
        if (json_is_string(id))
        {
            char const * name = json_string_value(id);        
            struct wf_mountpoint * mountpoint = wf_impl_mountpoint_create(context->local_path);
            protocol->filesystem = wf_impl_filesystem_create(protocol->wsi,protocol->proxy, name, mountpoint);
            if (NULL != protocol->filesystem)
            {
                reason = WF_CLIENT_FILESYSTEM_ADDED;
            }
            else
            {
                wf_impl_mountpoint_dispose(mountpoint);
            }
        }
    }

    free(context->local_path);
    free(context);
    protocol->callback(protocol->user_data, reason, NULL);
}

static int wf_impl_client_protocol_lws_callback(
	struct lws * wsi,
	enum lws_callback_reasons reason,
	void * WF_UNUSED_PARAM(user),
	void * in,
	size_t WF_UNUSED_PARAM(len))
{
    int result = 0;
    struct lws_protocols const * ws_protocol = lws_get_protocol(wsi);
    struct wf_client_protocol * protocol = (NULL != ws_protocol) ? ws_protocol->user : NULL;

    if (NULL != protocol)
    {
        wf_impl_timer_manager_check(protocol->timer_manager);

        switch (reason)
        {
            case LWS_CALLBACK_CLIENT_ESTABLISHED:
                protocol->is_connected = true;
                protocol->callback(protocol->user_data, WF_CLIENT_CONNECTED, NULL);
                break;
            case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
                protocol->is_connected = false;
                protocol->wsi = NULL;
                protocol->callback(protocol->user_data, WF_CLIENT_DISCONNECTED, NULL);
                break;
            case LWS_CALLBACK_CLIENT_CLOSED:
                protocol->is_connected = false;
                protocol->wsi = NULL;
                protocol->callback(protocol->user_data, WF_CLIENT_DISCONNECTED, NULL);
                break;
            case LWS_CALLBACK_CLIENT_RECEIVE:
                wf_impl_client_protocol_receive(protocol, in, len, lws_is_final_fragment(wsi));
                break;
            case LWS_CALLBACK_SERVER_WRITEABLE:
                // fall-through
            case LWS_CALLBACK_CLIENT_WRITEABLE:
                if (wsi == protocol->wsi)
                {
                    if (protocol->is_shutdown_requested)
                    {
                        result = 1;
                    }
                    else if (!wf_impl_slist_empty(&protocol->messages))
                    {
                        struct wf_slist_item * item = wf_impl_slist_remove_first(&protocol->messages);
                        struct wf_message * message = wf_container_of(item, struct wf_message, item);
                        lws_write(wsi, (unsigned char*) message->data, message->length, LWS_WRITE_TEXT);
                        wf_impl_message_dispose(message);

                        if (!wf_impl_slist_empty(&protocol->messages))
                        {
                            lws_callback_on_writable(wsi);
                        }
                    }
                }
                break;
            case LWS_CALLBACK_RAW_RX_FILE:
                if ((NULL != protocol->filesystem) && (wsi == protocol->filesystem->wsi))
                {
                    wf_impl_filesystem_process_request(protocol->filesystem);
                }
            default:
                break;
        }
    }

    return result;
}

void
wf_impl_client_protocol_init(
    struct wf_client_protocol * protocol,
    wf_client_protocol_callback_fn * callback,
    void * user_data)
{
    protocol->is_connected = false,
    protocol->is_shutdown_requested = false;
    protocol->wsi = NULL;
    protocol->callback = callback;
    protocol->user_data = user_data;
    protocol->filesystem = NULL;

    wf_impl_buffer_init(&protocol->recv_buffer, WF_DEFAULT_MESSAGE_SIZE);
    wf_impl_slist_init(&protocol->messages);
    protocol->timer_manager = wf_impl_timer_manager_create();
    protocol->proxy = wf_impl_jsonrpc_proxy_create(protocol->timer_manager, WF_DEFAULT_TIMEOUT, &wf_impl_client_protocol_send, protocol);

    protocol->callback(protocol->user_data, WF_CLIENT_INIT, NULL);
}

void
wf_impl_client_protocol_cleanup(
    struct wf_client_protocol * protocol)
{
    protocol->callback(protocol->user_data, WF_CLIENT_CLEANUP, NULL);

    wf_impl_jsonrpc_proxy_dispose(protocol->proxy);
    wf_impl_timer_manager_dispose(protocol->timer_manager);
    wf_impl_message_queue_cleanup(&protocol->messages);

    if (NULL != protocol->filesystem)
    {
        wf_impl_filesystem_dispose(protocol->filesystem);
        protocol->filesystem = NULL;
    }

    wf_impl_buffer_cleanup(&protocol->recv_buffer);
}

void
wf_impl_client_protocol_callback(
    struct wf_client_protocol * protocol,
    int reason,
    void * arg)
{
    protocol->callback(protocol->user_data, reason, arg);
}

void
wf_impl_client_protocol_init_lws(
    struct wf_client_protocol * protocol,
    struct lws_protocols * lws_protocol)
{
    lws_protocol->name = WF_PROTOCOL_NAME_ADAPTER_CLIENT;
    lws_protocol->callback = &wf_impl_client_protocol_lws_callback;
    lws_protocol->per_session_data_size = 0;
    lws_protocol->user = protocol;
}

void
wf_impl_client_protocol_connect(
    struct wf_client_protocol * protocol,
    struct lws_context * context,
    char const * url)
{
    struct wf_url url_data;
    bool const success = wf_impl_url_init(&url_data, url);
    if (success)
    {
        struct lws_client_connect_info info;
        memset(&info, 0 ,sizeof(struct lws_client_connect_info));
        info.context = context;
        info.port = url_data.port;
        info.address = url_data.host;
        info.path = url_data.path;
        info.host = info.address;
        info.origin = info.address;
        info.ssl_connection = (url_data.use_tls) ? LCCSCF_USE_SSL : 0;
        info.protocol = WF_PROTOCOL_NAME_PROVIDER_SERVER;
        info.local_protocol_name = WF_PROTOCOL_NAME_ADAPTER_CLIENT;
        info.pwsi = &protocol->wsi;

        lws_client_connect_via_info(&info);
        wf_impl_url_cleanup(&url_data);
    }
    else
    {
        protocol->callback(protocol->user_data, WF_CLIENT_DISCONNECTED, NULL);
    }
}

void
wf_impl_client_protocol_disconnect(
    struct wf_client_protocol * protocol)
{
    if (protocol->is_connected)
    {
        protocol->is_shutdown_requested = true;
        lws_callback_on_writable(protocol->wsi);
    }
    else
    {
        protocol->callback(protocol->user_data, WF_CLIENT_DISCONNECTED, NULL);
    }
    
}

void
wf_impl_client_protocol_authenticate(
    struct wf_client_protocol * protocol)
{
    struct wf_credentials creds;
    wf_impl_credentials_init_default(&creds);
    protocol->callback(protocol->user_data, WF_CLIENT_AUTHENTICATE_GET_CREDENTIALS, &creds);

    wf_impl_jsonrpc_proxy_invoke(
        protocol->proxy,
        &wf_impl_client_protocol_on_authenticate_finished,
        protocol,
        "authenticate",
        "sj",
        creds.type, &wf_impl_credentials_write, &creds);

    wf_impl_credentials_cleanup(&creds);
}

void
wf_impl_client_protocol_add_filesystem(
    struct wf_client_protocol * protocol,
    char const * local_path,
    char const * name)
{
    if (NULL == protocol->filesystem)
    {
        struct wf_impl_client_protocol_add_filesystem_context * context = malloc(sizeof(struct wf_impl_client_protocol_add_filesystem_context));
        context->protocol = protocol;
        context->local_path = strdup(local_path);

        wf_impl_jsonrpc_proxy_invoke(
            protocol->proxy,
            &wf_impl_client_protocol_on_add_filesystem_finished,
            context,
            "add_filesystem",
            "s",
            name);
    }
    else
    {
        protocol->callback(protocol->user_data, WF_CLIENT_FILESYSTEM_ADD_FAILED, NULL);
    }
}

