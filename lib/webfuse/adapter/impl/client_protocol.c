#include "webfuse/adapter/impl/client_protocol.h"
#include "webfuse/adapter/client_callback.h"
#include "webfuse/core/protocol_names.h"
#include "webfuse/core/url.h"
#include "webfuse/core/util.h"

#include <stddef.h>
#include <libwebsockets.h>

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
        switch (reason)
        {
            case LWS_CALLBACK_CLIENT_ESTABLISHED:
                protocol->is_connected = true;
                protocol->callback(protocol->user_data, WF_CLIENT_CONNECTED, NULL);
                break;
            case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
                protocol->is_connected = false;
                protocol->callback(protocol->user_data, WF_CLIENT_DISCONNECTED, NULL);
                break;
            case LWS_CALLBACK_CLIENT_CLOSED:
                protocol->is_connected = false;
                protocol->callback(protocol->user_data, WF_CLIENT_DISCONNECTED, NULL);
                protocol->wsi = NULL;
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
    wf_client_callback_fn * callback,
    void * user_data)
{
    protocol->is_connected = false,
    protocol->is_shutdown_requested = false;
    protocol->wsi = NULL;
    protocol->callback = callback;
    protocol->user_data = user_data;
    protocol->callback(protocol->user_data, WF_CLIENT_INIT, NULL);
}

void
wf_impl_client_protocol_cleanup(
    struct wf_client_protocol * protocol)
{
    protocol->callback(protocol->user_data, WF_CLIENT_CLEANUP, NULL);
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
    bool const success = wf_url_init(&url_data, url);
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
        wf_url_cleanup(&url_data);
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
