#include "webfuse/adapter/impl/session.h"
#include "webfuse/adapter/impl/authenticators.h"
#include "webfuse/core/message_queue.h"
#include "webfuse/core/message.h"
#include "webfuse/adapter/impl/mountpoint_factory.h"
#include "webfuse/adapter/impl/mountpoint.h"

#include "webfuse/core/container_of.h"
#include "webfuse/core/util.h"

#include "webfuse/core/jsonrpc/proxy.h"
#include "webfuse/core/jsonrpc/request.h"
#include "webfuse/core/jsonrpc/response.h"

#include <libwebsockets.h>
#include <stddef.h>
#include <stdlib.h>

#define WF_DEFAULT_TIMEOUT (10 * 1000)

static bool wf_impl_session_send(
    json_t * request,
    void * user_data)
{
    struct wf_impl_session * session = user_data;
    struct wf_message * message = wf_message_create(request);

    bool result = (session->is_authenticated || wf_jsonrpc_is_response(request)) && (NULL != session->wsi);

    if (result)
    {
        wf_slist_append(&session->messages, &message->item);
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
    struct wf_timer_manager * timer_manager,
    struct wf_jsonrpc_server * server,
    struct wf_impl_mountpoint_factory * mountpoint_factory)
{

    struct wf_impl_session * session = malloc(sizeof(struct wf_impl_session));
    wf_slist_init(&session->filesystems);
    
    session->wsi = wsi;
    session->is_authenticated = false;
    session->authenticators = authenticators;
    session->server = server;
    session->mountpoint_factory = mountpoint_factory;
    session->rpc = wf_jsonrpc_proxy_create(timer_manager, WF_DEFAULT_TIMEOUT, &wf_impl_session_send, session);
    wf_slist_init(&session->messages);

    return session;
}

static void wf_impl_session_dispose_filesystems(
    struct wf_slist * filesystems)
{    
    struct wf_slist_item * item = wf_slist_first(filesystems);
    while (NULL != item)
    {
        struct wf_slist_item * next = item->next;
        struct wf_impl_filesystem * filesystem = wf_container_of(item, struct wf_impl_filesystem, item);
        wf_impl_filesystem_dispose(filesystem);
        
        item = next;
    }
}

void wf_impl_session_dispose(
    struct wf_impl_session * session)
{
    wf_jsonrpc_proxy_dispose(session->rpc);
    wf_message_queue_cleanup(&session->messages);

    wf_impl_session_dispose_filesystems(&session->filesystems);
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
    bool result;

    struct wf_mountpoint * mountpoint = wf_impl_mountpoint_factory_create_mountpoint(session->mountpoint_factory, name);
    result = (NULL != mountpoint);
 
    if (result)
    {
        struct wf_impl_filesystem * filesystem = wf_impl_filesystem_create(session->wsi, session->rpc, name, mountpoint);
        result = (NULL != filesystem);
        if (result)
        {
            wf_slist_append(&session->filesystems, &filesystem->item);
        }
    }
    
    // cleanup on error
    if (!result)
    {
        if (NULL != mountpoint)
        {
            wf_impl_mountpoint_dispose(mountpoint);
        }
    }

    return result;
}


void wf_impl_session_onwritable(
    struct wf_impl_session * session)
{
    if (!wf_slist_empty(&session->messages))
    {
        struct wf_slist_item * item = wf_slist_remove_first(&session->messages);                
        struct wf_message * message = wf_container_of(item, struct wf_message, item);
        lws_write(session->wsi, (unsigned char*) message->data, message->length, LWS_WRITE_TEXT);
        wf_message_dispose(message);

        if (!wf_slist_empty(&session->messages))
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
        if (wf_jsonrpc_is_response(message))
        {
            wf_jsonrpc_proxy_onresult(session->rpc, message);
        }
        else if (wf_jsonrpc_is_request(message))
        {
            wf_jsonrpc_server_process(session->server, message, &wf_impl_session_send, session);
        }

	    json_decref(message);
    }

}

static struct wf_impl_filesystem * wf_impl_session_get_filesystem(
    struct wf_impl_session * session,
    struct lws * wsi)
{
    struct wf_impl_filesystem * result = NULL;

    struct wf_slist_item * item = wf_slist_first(&session->filesystems);
    while (NULL != item)
    {
        struct wf_slist_item * next = item->next;
        struct wf_impl_filesystem * filesystem = wf_container_of(item, struct wf_impl_filesystem, item);
        if (wsi == filesystem->wsi)
        {
            result = filesystem;
            break;
        }

        item = next;
    }

    return result;
}


bool wf_impl_session_contains_wsi(
    struct wf_impl_session * session,
    struct lws * wsi)
{
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
