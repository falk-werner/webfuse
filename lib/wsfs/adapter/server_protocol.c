#include "wsfs/adapter/server_protocol_intern.h"

#include <stdlib.h>
#include <libwebsockets.h>

#include "wsfs/message.h"
#include "wsfs/util.h"

#include "wsfs/adapter/filesystem.h"

static int wsfs_server_protocol_callback(
	struct lws * wsi,
	enum lws_callback_reasons reason,
	void * WSFS_UNUSED_PARAM(user),
	void * in,
	size_t len)
{
    struct lws_protocols const * ws_protocol = lws_get_protocol(wsi);
    struct wsfs_server_protocol * protocol = ws_protocol->user;

    wsfs_timeout_manager_check(&protocol->timeout_manager);
    struct wsfs_session * session = wsfs_session_manager_get(&protocol->session_manager, wsi);

    switch (reason)
    {
        case LWS_CALLBACK_PROTOCOL_INIT:
            {            
                lws_sock_file_fd_type fd;
                fd.filefd = wsfs_filesystem_get_fd(&protocol->filesystem);
                if (!lws_adopt_descriptor_vhost(lws_get_vhost(wsi), LWS_ADOPT_RAW_FILE_DESC, fd, ws_protocol->name, NULL))
                {
                    fprintf(stderr, "error: unable to adopt fd");
                }
            }
            break;
		case LWS_CALLBACK_ESTABLISHED:
			{
                bool is_authenticated = wsfs_authenticators_authenticate(&protocol->authenticators, NULL);
                wsfs_session_manager_add(&protocol->session_manager, wsi, is_authenticated);
			}
    		break;
		case LWS_CALLBACK_CLOSED:
            {
                wsfs_session_manager_remove(&protocol->session_manager, wsi);
            }
            break;
		case LWS_CALLBACK_SERVER_WRITEABLE:
			if ((NULL != session) && (!wsfs_message_queue_empty(&session->queue)))
			{                
				struct wsfs_message * message = wsfs_message_queue_pop(&session->queue);
				lws_write(wsi, (unsigned char*) message->data, message->length, LWS_WRITE_TEXT);
				wsfs_message_dispose(message);

                if (!wsfs_message_queue_empty(&session->queue))
                {
                    lws_callback_on_writable(wsi);
                }
			}
    		break;
        case LWS_CALLBACK_RECEIVE:
            wsfs_jsonrpc_server_onresult(&protocol->rpc, in, len);
            break;
        case LWS_CALLBACK_RAW_RX_FILE:
            wsfs_filesystem_process_request(&protocol->filesystem);
            break;
        default:
            break;
    }

    return 0;
}

static bool wsfs_server_protocol_invoke(
    void * user_data,
    json_t const * request)
{
    bool result = false;
    struct wsfs_server_protocol * protocol = user_data;
    struct wsfs_session * session = &protocol->session_manager.session;

    if ((session->is_authenticated) && (NULL != session->wsi))
    {
        struct wsfs_message * message = wsfs_message_create(request);
        if (NULL != message)
        {
            wsfs_message_queue_push(&session->queue, message);
            lws_callback_on_writable(session->wsi);

            result = true;
        }
    }

    return result;
}


struct wsfs_server_protocol * wsfs_server_protocol_create(
    char * mount_point)
{
    struct wsfs_server_protocol * protocol = malloc(sizeof(struct wsfs_server_protocol));
    if (NULL != protocol)
    {
        if (!wsfs_server_protocol_init(protocol, mount_point))
        {
            free(protocol);
            protocol = NULL;
        }
    }

    return protocol;
}

void wsfs_server_protocol_dispose(
    struct wsfs_server_protocol * protocol)
{
    wsfs_server_protocol_cleanup(protocol);
    free(protocol);
}

void wsfs_server_protocol_init_lws(
    struct wsfs_server_protocol * protocol,
    struct lws_protocols * lws_protocol)
{
	lws_protocol->callback = &wsfs_server_protocol_callback;
	lws_protocol->per_session_data_size = 0;
	lws_protocol->user = protocol;
}

bool wsfs_server_protocol_init(
    struct wsfs_server_protocol * protocol,
    char * mount_point)
{
    wsfs_timeout_manager_init(&protocol->timeout_manager);
    wsfs_session_manager_init(&protocol->session_manager);
    wsfs_authenticators_init(&protocol->authenticators);

    wsfs_jsonrpc_server_init(&protocol->rpc, &protocol->timeout_manager);
    wsfs_jsonrpc_server_add(&protocol->rpc, "lookup", &wsfs_server_protocol_invoke, protocol);
    wsfs_jsonrpc_server_add(&protocol->rpc, "getattr", &wsfs_server_protocol_invoke, protocol);
    wsfs_jsonrpc_server_add(&protocol->rpc, "readdir", &wsfs_server_protocol_invoke, protocol);
    wsfs_jsonrpc_server_add(&protocol->rpc, "open", &wsfs_server_protocol_invoke, protocol);
    wsfs_jsonrpc_server_add(&protocol->rpc, "close", &wsfs_server_protocol_invoke, protocol);
    wsfs_jsonrpc_server_add(&protocol->rpc, "read", &wsfs_server_protocol_invoke, protocol);

    bool const success = wsfs_filesystem_init(&protocol->filesystem, &protocol->rpc, mount_point);

    // cleanup on error
    if (!success)
    {
        wsfs_jsonrpc_server_cleanup(&protocol->rpc);
        wsfs_authenticators_cleanup(&protocol->authenticators);
        wsfs_timeout_manager_cleanup(&protocol->timeout_manager);
        wsfs_session_manager_cleanup(&protocol->session_manager);
    }

    return success;
}

void wsfs_server_protocol_cleanup(
    struct wsfs_server_protocol * protocol)
{
    wsfs_filesystem_cleanup(&protocol->filesystem);
    wsfs_jsonrpc_server_cleanup(&protocol->rpc);
    wsfs_timeout_manager_cleanup(&protocol->timeout_manager);
    wsfs_authenticators_cleanup(&protocol->authenticators);
    wsfs_session_manager_cleanup(&protocol->session_manager);
}

void wsfs_server_protocol_add_authenticator(
    struct wsfs_server_protocol * protocol,
    char const * type,
    wsfs_authenticate_fn * authenticate,
    void * user_data)
{
    wsfs_authenticators_add(&protocol->authenticators, type, authenticate, user_data);
}
