#include "webfuse/adapter/impl/session.h"
#include "webfuse/adapter/impl/authenticators.h"
#include "webfuse/core/message_queue.h"
#include "webfuse/core/message.h"
#include "webfuse/adapter/impl/jsonrpc/server.h"

#include <libwebsockets.h>
#include <stddef.h>

void wf_impl_session_init(
    struct wf_impl_session * session,
    struct lws * wsi,
    struct wf_impl_authenticators * authenticators,
    struct wf_impl_jsonrpc_server * rpc)
 {
    session->wsi = wsi;
    session->is_authenticated = false;
    session->authenticators = authenticators;
    session->rpc = rpc;
    wf_message_queue_init(&session->queue);
 }

void wf_impl_session_cleanup(
    struct wf_impl_session * session)
{
    wf_message_queue_cleanup(&session->queue);
    session->is_authenticated = false;
    session->wsi = NULL;
    session->authenticators = NULL;
    session->rpc = NULL;
}

void wf_impl_session_authenticate(
    struct wf_impl_session * session,
    struct wf_credentials * creds)
{
    session->is_authenticated = wf_impl_authenticators_authenticate(session->authenticators, creds);
}

bool wf_impl_session_send(
    struct wf_impl_session * session,
    struct wf_message * message)
{
    bool result = (session->is_authenticated) && (NULL != session->wsi);

    if (result)
    {
        wf_message_queue_push(&session->queue, message);
        lws_callback_on_writable(session->wsi);

        result = true;
    }
    else
    {
        wf_message_dispose(message);
    }

    return result;
}

void wf_impl_session_onwritable(
    struct wf_impl_session * session)
{
    if (!wf_message_queue_empty(&session->queue))
    {                
        struct wf_message * message = wf_message_queue_pop(&session->queue);
        lws_write(session->wsi, (unsigned char*) message->data, message->length, LWS_WRITE_TEXT);
        wf_message_dispose(message);

        if (!wf_message_queue_empty(&session->queue))
        {
            lws_callback_on_writable(session->wsi);
        }
    }
}


void wf_impl_session_receive(
    struct wf_impl_session * session,
    char const * data,
    size_t length)
{
    wf_impl_jsonrpc_server_onresult(session->rpc, data, length);
}