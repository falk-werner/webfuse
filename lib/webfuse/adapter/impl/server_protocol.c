#include "webfuse/adapter/impl/server_protocol.h"

#include <stdlib.h>
#include <libwebsockets.h>

#include "webfuse/core/message.h"
#include "webfuse/core/util.h"

#include "webfuse/adapter/impl/filesystem.h"

static int wf_impl_server_protocol_callback(
	struct lws * wsi,
	enum lws_callback_reasons reason,
	void * WF_UNUSED_PARAM(user),
	void * in,
	size_t len)
{
    struct lws_protocols const * ws_protocol = lws_get_protocol(wsi);
    struct wf_server_protocol * protocol = ws_protocol->user;

    wf_impl_timeout_manager_check(&protocol->timeout_manager);
    struct wf_impl_session * session = wf_impl_session_manager_get(&protocol->session_manager, wsi);

    switch (reason)
    {
        case LWS_CALLBACK_PROTOCOL_INIT:
            {            
                lws_sock_file_fd_type fd;
                fd.filefd = wf_impl_filesystem_get_fd(&protocol->filesystem);
                if (!lws_adopt_descriptor_vhost(lws_get_vhost(wsi), LWS_ADOPT_RAW_FILE_DESC, fd, ws_protocol->name, NULL))
                {
                    fprintf(stderr, "error: unable to adopt fd");
                }
            }
            break;
		case LWS_CALLBACK_ESTABLISHED:
            session = wf_impl_session_manager_add(
                &protocol->session_manager,
                wsi,
                &protocol->authenticators,
                &protocol->rpc);

            if (NULL != session)
            {
                wf_impl_session_authenticate(session, NULL);
            }
    		break;
		case LWS_CALLBACK_CLOSED:
            wf_impl_session_manager_remove(&protocol->session_manager, wsi);
            break;
		case LWS_CALLBACK_SERVER_WRITEABLE:
			if (NULL != session)
			{                
                wf_impl_session_onwritable(session);
			}
    		break;
        case LWS_CALLBACK_RECEIVE:
            if (NULL != session)
            {
                wf_impl_session_receive(session, in, len);
            }
            break;
        case LWS_CALLBACK_RAW_RX_FILE:
            wf_impl_filesystem_process_request(&protocol->filesystem);
            break;
        default:
            break;
    }

    return 0;
}

static bool wf_impl_server_protocol_invoke(
    void * user_data,
    json_t const * request)
{
    struct wf_server_protocol * protocol = user_data;
    struct wf_impl_session * session = &protocol->session_manager.session;
    struct wf_message * message = wf_message_create(request);

    bool const result = wf_impl_session_send(session, message);

    return result;
}


struct wf_server_protocol * wf_impl_server_protocol_create(
    char * mount_point)
{
    struct wf_server_protocol * protocol = malloc(sizeof(struct wf_server_protocol));
    if (NULL != protocol)
    {
        if (!wf_impl_server_protocol_init(protocol, mount_point))
        {
            free(protocol);
            protocol = NULL;
        }
    }

    return protocol;
}

void wf_impl_server_protocol_dispose(
    struct wf_server_protocol * protocol)
{
    wf_impl_server_protocol_cleanup(protocol);
    free(protocol);
}

void wf_impl_server_protocol_init_lws(
    struct wf_server_protocol * protocol,
    struct lws_protocols * lws_protocol)
{
	lws_protocol->callback = &wf_impl_server_protocol_callback;
	lws_protocol->per_session_data_size = 0;
	lws_protocol->user = protocol;
}

bool wf_impl_server_protocol_init(
    struct wf_server_protocol * protocol,
    char * mount_point)
{
    wf_impl_timeout_manager_init(&protocol->timeout_manager);
    wf_impl_session_manager_init(&protocol->session_manager);
    wf_impl_authenticators_init(&protocol->authenticators);

    wf_impl_jsonrpc_server_init(&protocol->rpc, &protocol->timeout_manager);
    wf_impl_jsonrpc_server_add(&protocol->rpc, "lookup", &wf_impl_server_protocol_invoke, protocol);
    wf_impl_jsonrpc_server_add(&protocol->rpc, "getattr", &wf_impl_server_protocol_invoke, protocol);
    wf_impl_jsonrpc_server_add(&protocol->rpc, "readdir", &wf_impl_server_protocol_invoke, protocol);
    wf_impl_jsonrpc_server_add(&protocol->rpc, "open", &wf_impl_server_protocol_invoke, protocol);
    wf_impl_jsonrpc_server_add(&protocol->rpc, "close", &wf_impl_server_protocol_invoke, protocol);
    wf_impl_jsonrpc_server_add(&protocol->rpc, "read", &wf_impl_server_protocol_invoke, protocol);

    bool const success = wf_impl_filesystem_init(&protocol->filesystem, &protocol->rpc, mount_point);

    // cleanup on error
    if (!success)
    {
        wf_impl_jsonrpc_server_cleanup(&protocol->rpc);
        wf_impl_authenticators_cleanup(&protocol->authenticators);
        wf_impl_timeout_manager_cleanup(&protocol->timeout_manager);
        wf_impl_session_manager_cleanup(&protocol->session_manager);
    }

    return success;
}

void wf_impl_server_protocol_cleanup(
    struct wf_server_protocol * protocol)
{
    wf_impl_filesystem_cleanup(&protocol->filesystem);
    wf_impl_jsonrpc_server_cleanup(&protocol->rpc);
    wf_impl_timeout_manager_cleanup(&protocol->timeout_manager);
    wf_impl_authenticators_cleanup(&protocol->authenticators);
    wf_impl_session_manager_cleanup(&protocol->session_manager);
}

void wf_impl_server_protocol_add_authenticator(
    struct wf_server_protocol * protocol,
    char const * type,
    wf_authenticate_fn * authenticate,
    void * user_data)
{
    wf_impl_authenticators_add(&protocol->authenticators, type, authenticate, user_data);
}
