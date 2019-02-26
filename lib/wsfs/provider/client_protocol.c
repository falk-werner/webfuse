#include "wsfs/provider/client_protocol_intern.h"

#include <stdlib.h>
#include <string.h>

#include <libwebsockets.h>
#include <jansson.h>


#include "wsfs/provider/provider_intern.h"
#include "wsfs/provider/operation/lookup_intern.h"
#include "wsfs/provider/operation/getattr_intern.h"
#include "wsfs/provider/operation/readdir_intern.h"
#include "wsfs/provider/operation/open_intern.h"
#include "wsfs/provider/operation/close_intern.h"
#include "wsfs/provider/operation/read_intern.h"

#include "wsfs/util.h"
#include "wsfs/message.h"

static void wsfsp_client_protocol_respond(
    json_t * response,
    void * user_data)
{
    // ToDo: implment me
    (void) user_data;

    char * value = json_dumps(response, 0);
    if (NULL != value)
    {
        puts(value);
    }
    free(value);
}

static void wsfsp_client_protocol_process_request(
     struct wsfsp_client_protocol * protocol, 
     char const * message,
     size_t length)
{
    json_t * request = json_loadb(message, length, 0, NULL);
    if (NULL != request)
    {
        struct wsfsp_invokation_context context =
        {
            .provider = &protocol->provider,
            .user_data = protocol->user_data,
            .request = &protocol->request
        };

        puts("wsfsp_provider_invoke");
        wsfsp_provider_invoke(&context, request);
        json_decref(request);
    }
}


static int wsfsp_client_protocol_callback(
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
            puts("established");
            protocol->provider.connected(protocol->user_data);
            break;
        case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
            protocol->provider.disconnected(protocol->user_data);
            break;
        case LWS_CALLBACK_CLIENT_CLOSED:
            protocol->provider.connected(protocol->user_data);        
            break;
        case LWS_CALLBACK_CLIENT_RECEIVE:
            wsfsp_client_protocol_process_request(protocol, in, len);
            break;
        case LWS_CALLBACK_CLIENT_WRITEABLE:
		{
			if ((wsi == protocol->wsi) && (!wsfs_message_queue_empty(&protocol->queue)))
			{                
				struct wsfs_message * message = wsfs_message_queue_pop(&protocol->queue);
				lws_write(wsi, (unsigned char*) message->data, message->length, LWS_WRITE_TEXT);
				wsfs_message_dispose(message);
			}
		}
        default:
            break;            
        }

        if ((wsi == protocol->wsi) && (!wsfs_message_queue_empty(&protocol->queue)))
        {
            lws_callback_on_writable(wsi);
        }    
    }

    return 0;
}


void wsfsp_client_protocol_init(
    struct wsfsp_client_protocol * protocol,
    struct wsfsp_provider const * provider,
    void * user_data)
{
    wsfs_message_queue_init(&protocol->queue);

    protocol->wsi = NULL;

    protocol->request.respond = &wsfsp_client_protocol_respond;
    protocol->request.user_data = protocol;

    protocol->user_data = user_data;
    protocol->provider.lookup = (NULL != provider->lookup) ? provider->lookup : &wsfsp_lookup_default;
    protocol->provider.getattr = (NULL != provider->getattr) ? provider->getattr : &wsfsp_getattr_default;
    protocol->provider.readdir = (NULL != provider->readdir) ? provider->readdir : &wsfsp_readdir_default;
    protocol->provider.open = (NULL != provider->open) ? provider->open : &wsfsp_open_default;
    protocol->provider.close = (NULL != provider->close) ? provider->close : &wsfsp_close_default;
    protocol->provider.read = (NULL != provider->read) ? provider->read : &wsfsp_read_default;
    protocol->provider.connected = (NULL != provider->connected) ? provider->connected : &wsfsp_connected_default;
    protocol->provider.disconnected = (NULL != provider->disconnected) ? provider->disconnected : &wsfsp_disconnected_default;
    protocol->provider.ontimer = (NULL != provider->ontimer) ? provider->ontimer : &wsfsp_ontimer_default;
}

void wsfsp_client_protocol_cleanup(
    struct wsfsp_client_protocol * protocol)
{
    wsfs_message_queue_cleanup(&protocol->queue);
}

struct wsfsp_client_protocol * wsfsp_client_protocol_create(
    struct wsfsp_provider const * provider,
    void * user_data)
{
    struct wsfsp_client_protocol * protocol = malloc(sizeof(struct wsfsp_client_protocol));
    if (NULL != protocol)
    {
        wsfsp_client_protocol_init(protocol, provider, user_data);
    }

    return protocol;
}

void wsfsp_client_protocol_dispose(
    struct wsfsp_client_protocol * protocol)
{
    wsfsp_client_protocol_cleanup(protocol);
    free(protocol);
}

void wsfsp_client_protocol_init_lws(
    struct wsfsp_client_protocol * protocol,
    struct lws_protocols * lws_protocol)
{
	lws_protocol->callback = &wsfsp_client_protocol_callback;
	lws_protocol->per_session_data_size = 0;
	lws_protocol->user = protocol;
}
