#include "wsfs/adapter/impl/session.h"
#include "wsfs/adapter/impl/authenticators.h"
#include "wsfs/message_queue.h"
#include "wsfs/message.h"
#include "wsfs/adapter/impl/jsonrpc/server.h"

#include <libwebsockets.h>
#include <stddef.h>

void wsfs_session_init(
    struct wsfs_session * session,
    struct lws * wsi,
    struct wsfs_authenticators * authenticators,
    struct wsfs_jsonrpc_server * rpc)
 {
    session->wsi = wsi;
    session->is_authenticated = false;
    session->authenticators = authenticators;
    session->rpc = rpc;
    wsfs_message_queue_init(&session->queue);
 }

void wsfs_session_cleanup(
    struct wsfs_session * session)
{
    wsfs_message_queue_cleanup(&session->queue);
    session->is_authenticated = false;
    session->wsi = NULL;
    session->authenticators = NULL;
    session->rpc = NULL;
}

void wsfs_session_authenticate(
    struct wsfs_session * session,
    struct wsfs_credentials * creds)
{
    session->is_authenticated = wsfs_authenticators_authenticate(session->authenticators, creds);
}

bool wsfs_session_send(
    struct wsfs_session * session,
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

void wsfs_session_onwritable(
    struct wsfs_session * session)
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


void wsfs_session_receive(
    struct wsfs_session * session,
    char const * data,
    size_t length)
{
    wsfs_jsonrpc_server_onresult(session->rpc, data, length);
}