#include "webfuse/provider/impl/client_protocol.h"

#include <stdlib.h>
#include <string.h>

#include <libwebsockets.h>
#include <jansson.h>

#include "webfuse/provider/impl/client_config.h"
#include "webfuse/provider/impl/provider.h"
#include "webfuse/provider/impl/credentials.h"
#include "webfuse/core/util.h"
#include "webfuse/core/message.h"
#include "webfuse/core/message_queue.h"
#include "webfuse/core/container_of.h"
#include "webfuse/core/url.h"
#include "webfuse/core/protocol_names.h"

#include "webfuse/core/timer/manager.h"

#include "webfuse/core/jsonrpc/response.h"
#include "webfuse/core/jsonrpc/request.h"
#include "webfuse/core/jsonrpc/proxy.h"

#define WF_DEFAULT_TIMEOUT (10 * 1000)

static void wfp_impl_client_protocol_respond(
    json_t * response,
    void * user_data)
{
    struct wfp_client_protocol * protocol = (struct wfp_client_protocol *) user_data;

    struct wf_message * message = wf_message_create(response);
    if (NULL != message)
    {
        wf_slist_append(&protocol->messages, &message->item);
        lws_callback_on_writable(protocol->wsi);
    }
}

static void wfp_impl_client_protocol_process(
     struct wfp_client_protocol * protocol, 
     char const * data,
     size_t length)
{
    json_t * message = json_loadb(data, length, 0, NULL);
    if (NULL != message)
    {
        if (wf_jsonrpc_is_response(message))
        {
            wf_jsonrpc_proxy_onresult(protocol->proxy, message);
        }

        if (wf_jsonrpc_is_request(message))
        {
            struct wfp_impl_invokation_context context =
            {
                .provider = &protocol->provider,
                .user_data = protocol->user_data,
                .request = &protocol->request
            };

            wfp_impl_provider_invoke(&context, message);
        }

        json_decref(message);
    }
}

static void 
wfp_impl_client_protocol_on_add_filesystem_finished(
	void * user_data,
	json_t const * result,
	json_t const * WF_UNUSED_PARAM(error))    
{
    struct wfp_client_protocol * protocol = user_data;
    if (NULL == protocol->wsi) { return; }

    if (NULL != result)
    {
        protocol->is_connected = true;
        protocol->provider.connected(protocol->user_data);
    }
    else
    {
        protocol->is_shutdown_requested = true;
        lws_callback_on_writable(protocol->wsi);
    }    
}

static void wfp_impl_client_protocol_add_filesystem(
     struct wfp_client_protocol * protocol)
{
    wf_jsonrpc_proxy_invoke(
        protocol->proxy, 
        &wfp_impl_client_protocol_on_add_filesystem_finished,
        protocol,
        "add_filesystem",
        "s",
        "cprovider");
}

static void 
wfp_impl_client_protocol_on_authenticate_finished(
	void * user_data,
	json_t const * result,
	json_t const * WF_UNUSED_PARAM(error))    
{
    struct wfp_client_protocol * protocol = user_data;
    if (NULL == protocol->wsi) { return; }

    if (NULL != result)
    {
        wfp_impl_client_protocol_add_filesystem(protocol);
    }
    else
    {
        protocol->is_shutdown_requested = true;
        lws_callback_on_writable(protocol->wsi);
    }    
}

static void wfp_impl_client_protocol_authenticate(
    struct wfp_client_protocol * protocol)
{
    struct wfp_credentials credentials;
    wfp_impl_credentials_init(&credentials);

    protocol->provider.get_credentials(&credentials, protocol->user_data);

    char const * cred_type = wfp_impl_credentials_get_type(&credentials);
    json_t * creds = wfp_impl_credentials_get(&credentials);
    json_incref(creds);

    wf_jsonrpc_proxy_invoke(
        protocol->proxy, 
        &wfp_impl_client_protocol_on_authenticate_finished, 
        protocol, 
        "authenticate", 
        "sj",
        cred_type, creds);

    wfp_impl_credentials_cleanup(&credentials);
}

static void wfp_impl_client_protocol_handshake(
    struct wfp_client_protocol * protocol)
{
    if (wfp_impl_provider_is_authentication_enabled(&protocol->provider))
    {
        wfp_impl_client_protocol_authenticate(protocol);
    }
    else
    {
        wfp_impl_client_protocol_add_filesystem(protocol);
    }
}

static int wfp_impl_client_protocol_callback(
	struct lws * wsi,
	enum lws_callback_reasons reason,
	void * WF_UNUSED_PARAM(user),
	void * in,
	size_t len)
{
    int result = 0;
    struct lws_protocols const * ws_protocol = lws_get_protocol(wsi);     
    struct wfp_client_protocol * protocol = (NULL != ws_protocol) ? ws_protocol->user: NULL;

    if (NULL != protocol)
    {
        wf_timer_manager_check(protocol->timer_manager);

        switch (reason)
        {
        case LWS_CALLBACK_CLIENT_ESTABLISHED:
            wfp_impl_client_protocol_handshake(protocol);
            break;
        case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
            protocol->is_connected = false;
            protocol->provider.disconnected(protocol->user_data);
            break;
        case LWS_CALLBACK_CLIENT_CLOSED:
            protocol->is_connected = false;
            protocol->provider.disconnected(protocol->user_data);   
            protocol->wsi = NULL;
            break;
        case LWS_CALLBACK_CLIENT_RECEIVE:
            wfp_impl_client_protocol_process(protocol, in, len);
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
                else if (!wf_slist_empty(&protocol->messages))
                {
                    struct wf_slist_item * item = wf_slist_remove_first(&protocol->messages);
                    struct wf_message * message = wf_container_of(item, struct wf_message, item);
                    lws_write(wsi, (unsigned char*) message->data, message->length, LWS_WRITE_TEXT);
                    wf_message_dispose(message);

                    if (!wf_slist_empty(&protocol->messages))
                    {
                        lws_callback_on_writable(wsi);
                    }
                }
            }
            break;
        default:
            break;            
        }
    }

    return result;
}

static bool wfp_impl_client_protocol_send(
    json_t * request,
    void * user_data)
{
    bool result = false;
    struct wfp_client_protocol * protocol = user_data;

    struct wf_message * message = wf_message_create(request);
    if (NULL != message)
    {
        wf_slist_append(&protocol->messages, &message->item);
        lws_callback_on_writable(protocol->wsi);
        result = true;
    }

    return result;
}

void wfp_impl_client_protocol_init(
    struct wfp_client_protocol * protocol,
    struct wfp_provider const * provider,
    void * user_data)
{
    protocol->is_connected = false;
    protocol->is_shutdown_requested = false;
    wf_slist_init(&protocol->messages);

    protocol->wsi = NULL;

    protocol->request.respond = &wfp_impl_client_protocol_respond;
    protocol->request.user_data = protocol;

    protocol->timer_manager = wf_timer_manager_create();
    protocol->proxy = wf_jsonrpc_proxy_create(protocol->timer_manager, WF_DEFAULT_TIMEOUT, &wfp_impl_client_protocol_send, protocol);

    protocol->user_data = user_data;
    wfp_impl_provider_init_from_prototype(&protocol->provider, provider);
}

void wfp_impl_client_protocol_cleanup(
    struct wfp_client_protocol * protocol)
{
    wf_jsonrpc_proxy_dispose(protocol->proxy);
    wf_timer_manager_dispose(protocol->timer_manager);
    wf_message_queue_cleanup(&protocol->messages);
}

struct wfp_client_protocol * wfp_impl_client_protocol_create(
    struct wfp_client_config const * config)
{
    struct wfp_client_protocol * protocol = malloc(sizeof(struct wfp_client_protocol));
    wfp_impl_client_protocol_init(protocol, &config->provider, config->user_data);

    return protocol;
}

void wfp_impl_client_protocol_dispose(
    struct wfp_client_protocol * protocol)
{
    wfp_impl_client_protocol_cleanup(protocol);
    free(protocol);
}

void wfp_impl_client_protocol_init_lws(
    struct wfp_client_protocol * protocol,
    struct lws_protocols * lws_protocol)
{
    lws_protocol->name = WF_PROTOCOL_NAME_PROVIDER_CLIENT;
	lws_protocol->callback = &wfp_impl_client_protocol_callback;
	lws_protocol->per_session_data_size = 0;
	lws_protocol->user = protocol;
}

void wfp_impl_client_protocol_connect(
    struct wfp_client_protocol * protocol,
    struct lws_context * context,
    char const * url)
{
    struct wf_url url_data;
    bool const success = wf_url_init(&url_data, url);
    if (success)
    {
        struct lws_client_connect_info info;
        memset(&info, 0, sizeof(struct lws_client_connect_info));
        info.context = context;
        info.port = url_data.port;
        info.address = url_data.host;
        info.path = url_data.path;
        info.host = info.address;
        info.origin = info.address;
        info.ssl_connection = (url_data.use_tls) ? LCCSCF_USE_SSL : 0;
        info.protocol = WF_PROTOCOL_NAME_ADAPTER_SERVER;
        info.local_protocol_name = WF_PROTOCOL_NAME_PROVIDER_CLIENT;
        info.pwsi = &protocol->wsi;

        lws_client_connect_via_info(&info);

        wf_url_cleanup(&url_data);
    }
    else
    {
        protocol->provider.disconnected(protocol->user_data);
    }
    
}

void wfp_impl_client_protocol_disconnect(
    struct wfp_client_protocol * protocol)
{
    if (protocol->is_connected)
    {
        protocol->is_shutdown_requested = true;
        lws_callback_on_writable(protocol->wsi);
    }
    else
    {
        protocol->provider.disconnected(protocol->user_data);
    }
}
