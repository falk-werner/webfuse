#include "wsfs/provider/impl/client_protocol.h"

#include <stdlib.h>
#include <string.h>

#include <libwebsockets.h>
#include <jansson.h>


#include "wsfs/provider/impl/provider.h"
#include "wsfs/core/util.h"
#include "wsfs/core/message.h"

static void wsfsp_impl_client_protocol_respond(
    json_t * response,
    void * user_data)
{
    struct wsfsp_client_protocol * protocol = (struct wsfsp_client_protocol *) user_data;

    struct wsfs_message * message = wsfs_message_create(response);
    if (NULL != message)
    {
        wsfs_message_queue_push(&protocol->queue, message);
        lws_callback_on_writable(protocol->wsi);
    }
}

static void wsfsp_impl_client_protocol_process_request(
     struct wsfsp_client_protocol * protocol, 
     char const * message,
     size_t length)
{
    json_t * request = json_loadb(message, length, 0, NULL);
    if (NULL != request)
    {
        struct wsfsp_impl_invokation_context context =
        {
            .provider = &protocol->provider,
            .user_data = protocol->user_data,
            .request = &protocol->request
        };

        wsfsp_impl_provider_invoke(&context, request);
        json_decref(request);
    }
}


static int wsfsp_impl_client_protocol_callback(
	struct lws * wsi,
	enum lws_callback_reasons reason,
	void * WSFS_UNUSED_PARAM(user),
	void * in,
	size_t len)
{
    struct lws_protocols const * ws_protocol = lws_get_protocol(wsi);     
    struct wsfsp_client_protocol * protocol = (NULL != ws_protocol) ? ws_protocol->user: NULL;

    if (NULL != protocol)
    {
        switch (reason)
        {
        case LWS_CALLBACK_CLIENT_ESTABLISHED:
            protocol->provider.connected(protocol->user_data);
            break;
        case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
            protocol->provider.disconnected(protocol->user_data);
            break;
        case LWS_CALLBACK_CLIENT_CLOSED:
            protocol->provider.connected(protocol->user_data);        
            break;
        case LWS_CALLBACK_CLIENT_RECEIVE:
            wsfsp_impl_client_protocol_process_request(protocol, in, len);
            break;
        case LWS_CALLBACK_SERVER_WRITEABLE:
            // fall-through
        case LWS_CALLBACK_CLIENT_WRITEABLE:
			if ((wsi == protocol->wsi) && (!wsfs_message_queue_empty(&protocol->queue)))
			{                
				struct wsfs_message * message = wsfs_message_queue_pop(&protocol->queue);
				lws_write(wsi, (unsigned char*) message->data, message->length, LWS_WRITE_TEXT);
				wsfs_message_dispose(message);

                if (!wsfs_message_queue_empty(&protocol->queue))
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


void wsfsp_impl_client_protocol_init(
    struct wsfsp_client_protocol * protocol,
    struct wsfsp_provider const * provider,
    void * user_data)
{
    wsfs_message_queue_init(&protocol->queue);

    protocol->wsi = NULL;

    protocol->request.respond = &wsfsp_impl_client_protocol_respond;
    protocol->request.user_data = protocol;

    protocol->user_data = user_data;
    wsfsp_impl_provider_init_from_prototype(&protocol->provider, provider);
}

void wsfsp_impl_client_protocol_cleanup(
    struct wsfsp_client_protocol * protocol)
{
    wsfs_message_queue_cleanup(&protocol->queue);
}

struct wsfsp_client_protocol * wsfsp_impl_client_protocol_create(
    struct wsfsp_provider const * provider,
    void * user_data)
{
    struct wsfsp_client_protocol * protocol = malloc(sizeof(struct wsfsp_client_protocol));
    if (NULL != protocol)
    {
        wsfsp_impl_client_protocol_init(protocol, provider, user_data);
    }

    return protocol;
}

void wsfsp_impl_client_protocol_dispose(
    struct wsfsp_client_protocol * protocol)
{
    wsfsp_impl_client_protocol_cleanup(protocol);
    free(protocol);
}

void wsfsp_impl_client_protocol_init_lws(
    struct wsfsp_client_protocol * protocol,
    struct lws_protocols * lws_protocol)
{
	lws_protocol->callback = &wsfsp_impl_client_protocol_callback;
	lws_protocol->per_session_data_size = 0;
	lws_protocol->user = protocol;
}
