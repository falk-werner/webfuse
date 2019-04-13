#include "webfuse/adapter/impl/session.h"
#include "webfuse/adapter/impl/authenticators.h"
#include "webfuse/core/message_queue.h"
#include "webfuse/core/message.h"
#include "webfuse/adapter/impl/jsonrpc/proxy.h"
#include "webfuse/adapter/impl/jsonrpc/request.h"
#include "webfuse/adapter/impl/jsonrpc/response.h"

#include "webfuse/core/container_of.h"
#include "webfuse/core/util.h"

#include <libwebsockets.h>
#include <stddef.h>
#include <stdlib.h>
#include <limits.h>

static bool wf_impl_session_send(
    json_t * request,
    void * user_data)
{
    struct wf_impl_session * session = user_data;
    struct wf_message * message = wf_message_create(request);

    bool result = (session->is_authenticated || wf_impl_jsonrpc_is_response(request)) && (NULL != session->wsi);

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

struct wf_impl_session * wf_impl_session_create(
    struct lws * wsi,
    struct wf_impl_authenticators * authenticators,
    struct wf_impl_timeout_manager * timeout_manager,
    struct wf_impl_jsonrpc_server * server,
    char const * mount_point)
{

    struct wf_impl_session * session = malloc(sizeof(struct wf_impl_session));
    if (NULL != session)
    {
        wf_dlist_item_init(&session->item);
        wf_dlist_init(&session->filesystems);
        
        session->mount_point = strdup(mount_point);
        session->wsi = wsi;
        session->is_authenticated = false;
        session->authenticators = authenticators;
        session->server = server;
        wf_impl_jsonrpc_proxy_init(&session->rpc, timeout_manager, &wf_impl_session_send, session);
        wf_message_queue_init(&session->queue);
    }

    return session;
}

static void wf_impl_session_cleanup_filesystem(
    struct wf_dlist_item * item,
    void * WF_UNUSED_PARAM(user_data))
{    
    struct wf_impl_filesystem * filesystem = WF_CONTAINER_OF(item, struct wf_impl_filesystem, item);
    wf_impl_filesystem_dispose(filesystem);

}

void wf_impl_session_dispose(
    struct wf_impl_session * session)
{
    wf_dlist_cleanup(&session->filesystems, &wf_impl_session_cleanup_filesystem, NULL);

    wf_impl_jsonrpc_proxy_cleanup(&session->rpc);
    wf_message_queue_cleanup(&session->queue);
    session->is_authenticated = false;
    session->wsi = NULL;
    session->authenticators = NULL;
    session->server = NULL;
    free(session->mount_point);
    free(session);
} 

bool wf_impl_session_authenticate(
    struct wf_impl_session * session,
    struct wf_credentials * creds)
{
    session->is_authenticated = wf_impl_authenticators_authenticate(session->authenticators, creds);

    return session->is_authenticated;
}

bool wf_impl_session_add_filesystem(
    struct wf_impl_session * session,
    char const * name)
{
    struct wf_impl_filesystem * filesystem = wf_impl_filesystem_create(session, name);
    wf_dlist_prepend(&session->filesystems, &filesystem->item);
    return (NULL != filesystem);
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
    json_t * message = json_loadb(data, length, 0, NULL);
    if (NULL != message)
    {
        if (wf_impl_jsonrpc_is_response(message))
        {
            wf_impl_jsonrpc_proxy_onresult(&session->rpc, message);
        }
        else if (wf_impl_jsonrpc_is_request(message))
        {
            wf_impl_jsonrpc_server_process(session->server, message, &wf_impl_session_send, session);
        }

	    json_decref(message);
    }

}

static struct wf_impl_filesystem * wf_impl_session_get_filesystem(
    struct wf_impl_session * session,
    struct lws * wsi)
{
    struct wf_impl_filesystem * filesystem = WF_CONTAINER_OF(session->filesystems.first, struct wf_impl_filesystem, item);
    while (NULL != filesystem)
    {
        if (wsi == filesystem->wsi)
        {
            break;
        }
        else
        {
            filesystem = WF_CONTAINER_OF(filesystem->item.next, struct wf_impl_filesystem, item);
        }        
    }

    return filesystem;
}


bool wf_impl_session_contains_wsi(
    struct wf_dlist_item * item,
    void * user_data)
{
    struct lws * wsi = user_data;
    struct wf_impl_session * session = WF_CONTAINER_OF(item, struct wf_impl_session, item);

    bool const result = (NULL != wsi) && ((wsi == session->wsi) || (NULL != wf_impl_session_get_filesystem(session, wsi)));
    return result;
}


void wf_impl_session_process_filesystem_request(
    struct wf_impl_session * session, 
    struct lws * wsi)
{
    struct wf_impl_filesystem * filesystem = wf_impl_session_get_filesystem(session, wsi);
    if (NULL != filesystem)
    {
        wf_impl_filesystem_process_request(filesystem);
    }
}
