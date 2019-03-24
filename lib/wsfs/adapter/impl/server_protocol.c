#include "wsfs/adapter/impl/server_protocol.h"

#include <stdlib.h>
#include <libwebsockets.h>

#include "wsfs/message.h"
#include "wsfs/util.h"

#include "wsfs/adapter/impl/filesystem.h"

static int server_protocol_callback(
	struct lws * wsi,
	enum lws_callback_reasons reason,
	void * WSFS_UNUSED_PARAM(user),
	void * in,
	size_t len)
{
    struct lws_protocols const * ws_protocol = lws_get_protocol(wsi);
    struct server_protocol * protocol = ws_protocol->user;

    timeout_manager_check(&protocol->timeout_manager);
    struct session * session = session_manager_get(&protocol->session_manager, wsi);

    switch (reason)
    {
        case LWS_CALLBACK_PROTOCOL_INIT:
            {            
                lws_sock_file_fd_type fd;
                fd.filefd = filesystem_get_fd(&protocol->filesystem);
                if (!lws_adopt_descriptor_vhost(lws_get_vhost(wsi), LWS_ADOPT_RAW_FILE_DESC, fd, ws_protocol->name, NULL))
                {
                    fprintf(stderr, "error: unable to adopt fd");
                }
            }
            break;
		case LWS_CALLBACK_ESTABLISHED:
            session = session_manager_add(
                &protocol->session_manager,
                wsi,
                &protocol->authenticators,
                &protocol->rpc);

            if (NULL != session)
            {
                session_authenticate(session, NULL);
            }
    		break;
		case LWS_CALLBACK_CLOSED:
            session_manager_remove(&protocol->session_manager, wsi);
            break;
		case LWS_CALLBACK_SERVER_WRITEABLE:
			if (NULL != session)
			{                
                session_onwritable(session);
			}
    		break;
        case LWS_CALLBACK_RECEIVE:
            if (NULL != session)
            {
                session_receive(session, in, len);
            }
            break;
        case LWS_CALLBACK_RAW_RX_FILE:
            filesystem_process_request(&protocol->filesystem);
            break;
        default:
            break;
    }

    return 0;
}

static bool server_protocol_invoke(
    void * user_data,
    json_t const * request)
{
    struct server_protocol * protocol = user_data;
    struct session * session = &protocol->session_manager.session;
    struct wsfs_message * message = wsfs_message_create(request);

    bool const result = session_send(session, message);

    return result;
}


struct server_protocol * server_protocol_create(
    char * mount_point)
{
    struct server_protocol * protocol = malloc(sizeof(struct server_protocol));
    if (NULL != protocol)
    {
        if (!server_protocol_init(protocol, mount_point))
        {
            free(protocol);
            protocol = NULL;
        }
    }

    return protocol;
}

void server_protocol_dispose(
    struct server_protocol * protocol)
{
    server_protocol_cleanup(protocol);
    free(protocol);
}

void server_protocol_init_lws(
    struct server_protocol * protocol,
    struct lws_protocols * lws_protocol)
{
	lws_protocol->callback = &server_protocol_callback;
	lws_protocol->per_session_data_size = 0;
	lws_protocol->user = protocol;
}

bool server_protocol_init(
    struct server_protocol * protocol,
    char * mount_point)
{
    timeout_manager_init(&protocol->timeout_manager);
    session_manager_init(&protocol->session_manager);
    authenticators_init(&protocol->authenticators);

    jsonrpc_server_init(&protocol->rpc, &protocol->timeout_manager);
    jsonrpc_server_add(&protocol->rpc, "lookup", &server_protocol_invoke, protocol);
    jsonrpc_server_add(&protocol->rpc, "getattr", &server_protocol_invoke, protocol);
    jsonrpc_server_add(&protocol->rpc, "readdir", &server_protocol_invoke, protocol);
    jsonrpc_server_add(&protocol->rpc, "open", &server_protocol_invoke, protocol);
    jsonrpc_server_add(&protocol->rpc, "close", &server_protocol_invoke, protocol);
    jsonrpc_server_add(&protocol->rpc, "read", &server_protocol_invoke, protocol);

    bool const success = filesystem_init(&protocol->filesystem, &protocol->rpc, mount_point);

    // cleanup on error
    if (!success)
    {
        jsonrpc_server_cleanup(&protocol->rpc);
        authenticators_cleanup(&protocol->authenticators);
        timeout_manager_cleanup(&protocol->timeout_manager);
        session_manager_cleanup(&protocol->session_manager);
    }

    return success;
}

void server_protocol_cleanup(
    struct server_protocol * protocol)
{
    filesystem_cleanup(&protocol->filesystem);
    jsonrpc_server_cleanup(&protocol->rpc);
    timeout_manager_cleanup(&protocol->timeout_manager);
    authenticators_cleanup(&protocol->authenticators);
    session_manager_cleanup(&protocol->session_manager);
}

void server_protocol_add_authenticator(
    struct server_protocol * protocol,
    char const * type,
    authenticate_fn * authenticate,
    void * user_data)
{
    authenticators_add(&protocol->authenticators, type, authenticate, user_data);
}
