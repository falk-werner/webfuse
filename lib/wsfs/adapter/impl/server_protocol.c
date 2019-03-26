#include "wsfs/adapter/impl/server_protocol.h"

#include <stdlib.h>
#include <libwebsockets.h>

#include "wsfs/core/message.h"
#include "wsfs/core/util.h"

#include "wsfs/adapter/impl/filesystem.h"

static int wsfs_impl_server_protocol_callback(
	struct lws * wsi,
	enum lws_callback_reasons reason,
	void * WSFS_UNUSED_PARAM(user),
	void * in,
	size_t len)
{
    struct lws_protocols const * ws_protocol = lws_get_protocol(wsi);
    struct wsfs_server_protocol * protocol = ws_protocol->user;

    wsfs_impl_timeout_manager_check(&protocol->timeout_manager);
    struct wsfs_impl_session * session = wsfs_impl_session_manager_get(&protocol->session_manager, wsi);

    switch (reason)
    {
        case LWS_CALLBACK_PROTOCOL_INIT:
            {            
                lws_sock_file_fd_type fd;
                fd.filefd = wsfs_impl_filesystem_get_fd(&protocol->filesystem);
                if (!lws_adopt_descriptor_vhost(lws_get_vhost(wsi), LWS_ADOPT_RAW_FILE_DESC, fd, ws_protocol->name, NULL))
                {
                    fprintf(stderr, "error: unable to adopt fd");
                }
            }
            break;
		case LWS_CALLBACK_ESTABLISHED:
            session = wsfs_impl_session_manager_add(
                &protocol->session_manager,
                wsi,
                &protocol->authenticators,
                &protocol->rpc);

            if (NULL != session)
            {
                wsfs_impl_session_authenticate(session, NULL);
            }
    		break;
		case LWS_CALLBACK_CLOSED:
            wsfs_impl_session_manager_remove(&protocol->session_manager, wsi);
            break;
		case LWS_CALLBACK_SERVER_WRITEABLE:
			if (NULL != session)
			{                
                wsfs_impl_session_onwritable(session);
			}
    		break;
        case LWS_CALLBACK_RECEIVE:
            if (NULL != session)
            {
                wsfs_impl_session_receive(session, in, len);
            }
            break;
        case LWS_CALLBACK_RAW_RX_FILE:
            wsfs_impl_filesystem_process_request(&protocol->filesystem);
            break;
        default:
            break;
    }

    return 0;
}

static bool wsfs_impl_server_protocol_invoke(
    void * user_data,
    json_t const * request)
{
    struct wsfs_server_protocol * protocol = user_data;
    struct wsfs_impl_session * session = &protocol->session_manager.session;
    struct wsfs_message * message = wsfs_message_create(request);

    bool const result = wsfs_impl_session_send(session, message);

    return result;
}


struct wsfs_server_protocol * wsfs_impl_server_protocol_create(
    char * mount_point)
{
    struct wsfs_server_protocol * protocol = malloc(sizeof(struct wsfs_server_protocol));
    if (NULL != protocol)
    {
        if (!wsfs_impl_server_protocol_init(protocol, mount_point))
        {
            free(protocol);
            protocol = NULL;
        }
    }

    return protocol;
}

void wsfs_impl_server_protocol_dispose(
    struct wsfs_server_protocol * protocol)
{
    wsfs_impl_server_protocol_cleanup(protocol);
    free(protocol);
}

void wsfs_impl_server_protocol_init_lws(
    struct wsfs_server_protocol * protocol,
    struct lws_protocols * lws_protocol)
{
	lws_protocol->callback = &wsfs_impl_server_protocol_callback;
	lws_protocol->per_session_data_size = 0;
	lws_protocol->user = protocol;
}

bool wsfs_impl_server_protocol_init(
    struct wsfs_server_protocol * protocol,
    char * mount_point)
{
    wsfs_impl_timeout_manager_init(&protocol->timeout_manager);
    wsfs_impl_session_manager_init(&protocol->session_manager);
    wsfs_impl_authenticators_init(&protocol->authenticators);

    wsfs_impl_jsonrpc_server_init(&protocol->rpc, &protocol->timeout_manager);
    wsfs_impl_jsonrpc_server_add(&protocol->rpc, "lookup", &wsfs_impl_server_protocol_invoke, protocol);
    wsfs_impl_jsonrpc_server_add(&protocol->rpc, "getattr", &wsfs_impl_server_protocol_invoke, protocol);
    wsfs_impl_jsonrpc_server_add(&protocol->rpc, "readdir", &wsfs_impl_server_protocol_invoke, protocol);
    wsfs_impl_jsonrpc_server_add(&protocol->rpc, "open", &wsfs_impl_server_protocol_invoke, protocol);
    wsfs_impl_jsonrpc_server_add(&protocol->rpc, "close", &wsfs_impl_server_protocol_invoke, protocol);
    wsfs_impl_jsonrpc_server_add(&protocol->rpc, "read", &wsfs_impl_server_protocol_invoke, protocol);

    bool const success = wsfs_impl_filesystem_init(&protocol->filesystem, &protocol->rpc, mount_point);

    // cleanup on error
    if (!success)
    {
        wsfs_impl_jsonrpc_server_cleanup(&protocol->rpc);
        wsfs_impl_authenticators_cleanup(&protocol->authenticators);
        wsfs_impl_timeout_manager_cleanup(&protocol->timeout_manager);
        wsfs_impl_session_manager_cleanup(&protocol->session_manager);
    }

    return success;
}

void wsfs_impl_server_protocol_cleanup(
    struct wsfs_server_protocol * protocol)
{
    wsfs_impl_filesystem_cleanup(&protocol->filesystem);
    wsfs_impl_jsonrpc_server_cleanup(&protocol->rpc);
    wsfs_impl_timeout_manager_cleanup(&protocol->timeout_manager);
    wsfs_impl_authenticators_cleanup(&protocol->authenticators);
    wsfs_impl_session_manager_cleanup(&protocol->session_manager);
}

void wsfs_impl_server_protocol_add_authenticator(
    struct wsfs_server_protocol * protocol,
    char const * type,
    wsfs_authenticate_fn * authenticate,
    void * user_data)
{
    wsfs_impl_authenticators_add(&protocol->authenticators, type, authenticate, user_data);
}
