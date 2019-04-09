#include "webfuse/adapter/impl/session.h"
#include "webfuse/adapter/impl/authenticators.h"
#include "webfuse/core/message_queue.h"
#include "webfuse/core/message.h"
#include "webfuse/adapter/impl/jsonrpc/proxy.h"
#include "webfuse/adapter/impl/jsonrpc/request.h"
#include "webfuse/adapter/impl/jsonrpc/response.h"

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
    char const * mount_point,
    char const * protocol_name)
{
    static int session_id = 0;

    struct wf_impl_session * session = malloc(sizeof(struct wf_impl_session));
    if (NULL != session)
    {
        snprintf(session->mount_point, PATH_MAX, "%s/%d", mount_point, session_id);
        session_id++;
        mkdir(session->mount_point, 0755);

        wf_dlist_item_init(&session->item);
        
        session->wsi = wsi;
        session->is_authenticated = false;
        session->authenticators = authenticators;
        session->server = server;
        wf_impl_jsonrpc_proxy_init(&session->rpc, timeout_manager, &wf_impl_session_send, session);
        wf_message_queue_init(&session->queue);

        bool success = wf_impl_filesystem_init(&session->filesystem, session, session->mount_point);
        if (success)
        {
            lws_sock_file_fd_type fd;
            fd.filefd = wf_impl_filesystem_get_fd(&session->filesystem);
            session->wsi_fuse = lws_adopt_descriptor_vhost(lws_get_vhost(wsi), LWS_ADOPT_RAW_FILE_DESC, fd, protocol_name, wsi);
            if (NULL == session->wsi_fuse)
            {
                success = false;
                fprintf(stderr, "error: unable to adopt fd");
            }  
        }

        if (!success)
        {
            rmdir(session->mount_point);
            wf_impl_jsonrpc_proxy_cleanup(&session->rpc);
            wf_message_queue_cleanup(&session->queue);
            free(session);
            session = NULL;
        }
    }

    return session;
}

void wf_impl_session_dispose(
    struct wf_impl_session * session)
{
    wf_impl_filesystem_cleanup(&session->filesystem);
    rmdir(session->mount_point);

    wf_impl_jsonrpc_proxy_cleanup(&session->rpc);
    wf_message_queue_cleanup(&session->queue);
    session->is_authenticated = false;
    session->wsi = NULL;
    session->wsi_fuse = NULL;
    session->authenticators = NULL;
    session->server = NULL;
    free(session);
} 

bool wf_impl_session_authenticate(
    struct wf_impl_session * session,
    struct wf_credentials * creds)
{
    session->is_authenticated = wf_impl_authenticators_authenticate(session->authenticators, creds);

    return session->is_authenticated;
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