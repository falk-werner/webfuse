#include "webfuse/provider/impl/client_protocol.h"

#include <stdlib.h>
#include <string.h>

#include <libwebsockets.h>
#include <jansson.h>


#include "webfuse/provider/impl/provider.h"
#include "webfuse/core/util.h"
#include "webfuse/core/message.h"
#include "webfuse/core/message_queue.h"
#include "webfuse/core/container_of.h"

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

static void wfp_impl_client_protocol_process_request(
     struct wfp_client_protocol * protocol, 
     char const * message,
     size_t length)
{
    json_t * request = json_loadb(message, length, 0, NULL);
    if (NULL != request)
    {
        // FIXME: is_connected should be invoked, when filesystem added
        if ((!protocol->is_connected) && (NULL != json_object_get(request, "result")))
        {
            protocol->is_connected = true;
            protocol->provider.connected(protocol->user_data);
        }



        struct wfp_impl_invokation_context context =
        {
            .provider = &protocol->provider,
            .user_data = protocol->user_data,
            .request = &protocol->request
        };

        wfp_impl_provider_invoke(&context, request);
        json_decref(request);
    }
}

static void wfp_impl_client_protocol_add_filesystem(
     struct wfp_client_protocol * protocol)
{
    json_t * params = json_array();
    json_array_append_new(params, json_string("cprovider"));

    json_t * request = json_object();
    json_object_set_new(request, "method", json_string("add_filesystem"));
    json_object_set_new(request, "params", params);
    json_object_set_new(request, "id", json_integer(42));

    struct wf_message * message = wf_message_create(request);
    if (NULL != message)
    {
        wf_slist_append(&protocol->messages, &message->item);
        lws_callback_on_writable(protocol->wsi);
    }

    json_decref(request);
}

static int wfp_impl_client_protocol_callback(
	struct lws * wsi,
	enum lws_callback_reasons reason,
	void * WF_UNUSED_PARAM(user),
	void * in,
	size_t len)
{
    struct lws_protocols const * ws_protocol = lws_get_protocol(wsi);     
    struct wfp_client_protocol * protocol = (NULL != ws_protocol) ? ws_protocol->user: NULL;

    if (NULL != protocol)
    {
        switch (reason)
        {
        case LWS_CALLBACK_CLIENT_ESTABLISHED:
            wfp_impl_client_protocol_add_filesystem(protocol);
            // Defer is_connected until response received
            break;
        case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
            protocol->is_connected = false;
            protocol->provider.disconnected(protocol->user_data);
            break;
        case LWS_CALLBACK_CLIENT_CLOSED:
            protocol->is_connected = false;
            protocol->provider.disconnected(protocol->user_data);        
            break;
        case LWS_CALLBACK_CLIENT_RECEIVE:
            wfp_impl_client_protocol_process_request(protocol, in, len);
            break;
        case LWS_CALLBACK_SERVER_WRITEABLE:
            // fall-through
        case LWS_CALLBACK_CLIENT_WRITEABLE:
			if ((wsi == protocol->wsi) && (!wf_slist_empty(&protocol->messages)))
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
            break;
        default:
            break;            
        }
    }

    return 0;
}


void wfp_impl_client_protocol_init(
    struct wfp_client_protocol * protocol,
    struct wfp_provider const * provider,
    void * user_data)
{
    protocol->is_connected = false;
    wf_slist_init(&protocol->messages);

    protocol->wsi = NULL;

    protocol->request.respond = &wfp_impl_client_protocol_respond;
    protocol->request.user_data = protocol;

    protocol->user_data = user_data;
    wfp_impl_provider_init_from_prototype(&protocol->provider, provider);
}

void wfp_impl_client_protocol_cleanup(
    struct wfp_client_protocol * protocol)
{
    wf_message_queue_cleanup(&protocol->messages);
}

struct wfp_client_protocol * wfp_impl_client_protocol_create(
    struct wfp_provider const * provider,
    void * user_data)
{
    struct wfp_client_protocol * protocol = malloc(sizeof(struct wfp_client_protocol));
    if (NULL != protocol)
    {
        wfp_impl_client_protocol_init(protocol, provider, user_data);
    }

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
	lws_protocol->callback = &wfp_impl_client_protocol_callback;
	lws_protocol->per_session_data_size = 0;
	lws_protocol->user = protocol;
}
