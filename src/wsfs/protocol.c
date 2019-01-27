#include "wsfs/protocol.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <pthread.h>

#include "wsfs/util.h"
#include "wsfs/server.h"

struct wsfs_message
{
	char * content;
	size_t length;
};

struct wsfs_protocol
{
	pthread_mutex_t lock;
	struct lws * wsi;
	struct wsfs_message pending_message;
	struct wsfs_server * server;
};

static struct wsfs_protocol * wsfs_protocol_from_wsi(
	struct lws * wsi)
{
	struct lws_protocols const * protocol = lws_get_protocol(wsi);
	return protocol->user;
}

static bool wsfs_protocol_connect(
	struct wsfs_protocol * protocol,
	struct lws * wsi)
{
	pthread_mutex_lock(&protocol->lock);

	bool const success = (NULL == protocol->wsi);
	if (success)
	{
		protocol->wsi = wsi;
	}

	pthread_mutex_unlock(&protocol->lock);

	return success;
}

static bool wsfs_protocol_is_wsi_connected(
	struct wsfs_protocol * protocol,
	struct lws * wsi)
{
	pthread_mutex_lock(&protocol->lock);
	bool const result = (wsi == protocol->wsi);
	pthread_mutex_unlock(&protocol->lock);

	return result;
}

static void wsfs_protocol_disconnect(
	struct wsfs_protocol * protocol,
	struct lws * wsi)
{
	pthread_mutex_lock(&protocol->lock);
	if (wsi == protocol->wsi)
	{
		protocol->wsi = NULL;
	}
	pthread_mutex_unlock(&protocol->lock);
}

static void wsfs_protocol_get_message(
	struct wsfs_protocol * protocol, 
	struct wsfs_message * message)
{
	pthread_mutex_lock(&protocol->lock);

	message->content = protocol->pending_message.content;
	message->length = protocol->pending_message.length;
	
	protocol->pending_message.content = NULL;
	protocol->pending_message.length = 0;

	pthread_mutex_unlock(&protocol->lock);
}


static int wsfs_protocol_callback(
	struct lws *wsi,
	enum lws_callback_reasons reason,
	void * WSFS_UNUSED_PARAM(user),
	void *in,
	size_t len)
{
	int result = 0;
	struct wsfs_protocol * const protocol = wsfs_protocol_from_wsi(wsi);
	
	switch (reason)
	{
		case LWS_CALLBACK_ESTABLISHED:
		{
			if (!wsfs_protocol_connect(protocol, wsi))
			{
				puts("connect failed");
				lws_callback_on_writable(wsi);
				result = -1;
			}
		}
		break;
		case LWS_CALLBACK_CLOSED:
		{
			wsfs_protocol_disconnect(protocol, wsi);
		}
		break;
		case LWS_CALLBACK_SERVER_WRITEABLE:
		{
			if (wsfs_protocol_is_wsi_connected(protocol, wsi))
			{
				struct wsfs_message message;
				wsfs_protocol_get_message(protocol, &message);
				if (NULL != message.content)
				{
					lws_write(wsi, (unsigned char*) message.content, message.length, LWS_WRITE_TEXT);
					wsfs_protocol_message_dispose(message.content);
				}
			}
			else
			{
				result = -1;
			}
		}
		break;
		case LWS_CALLBACK_RECEIVE:
		{
			wsfs_server_handle_message(protocol->server, in, len);
		}
		break;
		default:
		break;
	}

	return result;
}


struct wsfs_protocol * wsfs_protocol_create(
	struct wsfs_server * server)
{
	struct wsfs_protocol * protocol = malloc(sizeof(struct wsfs_protocol));
	if (NULL != protocol)
	{	
		pthread_mutex_init(&protocol->lock, NULL);
		protocol->wsi = NULL;
		protocol->pending_message.content = NULL;
		protocol->pending_message.length = 0;
		protocol->server = server;
	}

	return protocol;
}

void wsfs_protocol_dispose(
	struct wsfs_protocol * protocol)
{
	pthread_mutex_destroy(&protocol->lock);

	if (NULL != protocol->pending_message.content)
	{
		wsfs_protocol_message_dispose(protocol->pending_message.content);
		protocol->pending_message.content = NULL;
	}

	free(protocol);
}

void wsfs_protocol_check(
	struct wsfs_protocol * protocol)
{
	pthread_mutex_lock(&protocol->lock);

	if ((NULL != protocol->wsi) && (NULL != protocol->pending_message.content))
	{
		lws_callback_on_writable(protocol->wsi);
	}

	pthread_mutex_unlock(&protocol->lock);
}

void wsfs_protocol_init_lws(
	struct wsfs_protocol * protocol,
	struct lws_protocols * lws_protocol)
{
	lws_protocol->callback = &wsfs_protocol_callback;
	lws_protocol->per_session_data_size = 1;
	lws_protocol->user = protocol;
}

char * wsfs_protocol_message_create(
	size_t size)
{
	char * buffer = malloc(LWS_PRE + size);
	return &buffer[LWS_PRE];
}

void wsfs_protocol_message_dispose(
	char * message)
{
	char * buffer = message - LWS_PRE;
	free(buffer);
}

bool wsfs_protocol_send(
	char * message,
	size_t length,
	void * user_data)
{
	struct wsfs_protocol * protocol = user_data;

	pthread_mutex_lock(&protocol->lock);

	struct wsfs_server * server = protocol->server;
	bool result = (NULL != protocol->wsi) && (NULL == protocol->pending_message.content);
	if (result)
	{
		protocol->pending_message.content = message;
		protocol->pending_message.length = length;
	}
	else
	{
		wsfs_protocol_message_dispose(message);
	}

	pthread_mutex_unlock(&protocol->lock);	

	if (result)
	{
		wsfs_server_wakeup(server);
	}

	return result;
}

