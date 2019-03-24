#include "wsfs/adapter/impl/session.h"
#include "wsfs/adapter/impl/authenticators.h"
#include "wsfs/message_queue.h"
#include "wsfs/message.h"
#include "wsfs/adapter/impl/jsonrpc/server.h"

#include <libwebsockets.h>
#include <stddef.h>

void session_init(
    struct session * session,
    struct lws * wsi,
    struct authenticators * authenticators,
    struct jsonrpc_server * rpc)
 {
    session->wsi = wsi;
    session->is_authenticated = false;
    session->authenticators = authenticators;
    session->rpc = rpc;
    wsfs_message_queue_init(&session->queue);
 }

void session_cleanup(
    struct session * session)
{
    wsfs_message_queue_cleanup(&session->queue);
    session->is_authenticated = false;
    session->wsi = NULL;
    session->authenticators = NULL;
    session->rpc = NULL;
}

void session_authenticate(
    struct session * session,
    struct wsfs_credentials * creds)
{
    session->is_authenticated = authenticators_authenticate(session->authenticators, creds);
}

bool session_send(
    struct session * session,
    struct wsfs_message * message)
{
    bool result = (session->is_authenticated) && (NULL != session->wsi);

    if (result)
    {
        wsfs_message_queue_push(&session->queue, message);
        lws_callback_on_writable(session->wsi);

        result = true;
    }
    else
    {
        wsfs_message_dispose(message);
    }

    return result;
}

void session_onwritable(
    struct session * session)
{
    if (!wsfs_message_queue_empty(&session->queue))
    {                
        struct wsfs_message * message = wsfs_message_queue_pop(&session->queue);
        lws_write(session->wsi, (unsigned char*) message->data, message->length, LWS_WRITE_TEXT);
        wsfs_message_dispose(message);

        if (!wsfs_message_queue_empty(&session->queue))
        {
            lws_callback_on_writable(session->wsi);
        }
    }
}


void session_receive(
    struct session * session,
    char const * data,
    size_t length)
{
    jsonrpc_server_onresult(session->rpc, data, length);
}