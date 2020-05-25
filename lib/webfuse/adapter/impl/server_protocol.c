#include "webfuse/adapter/impl/server_protocol.h"

#include <stdlib.h>
#include <ctype.h>
#include <libwebsockets.h>

#include "webfuse/core/message.h"
#include "webfuse/core/util.h"
#include "webfuse/core/protocol_names.h"

#include "webfuse/adapter/impl/credentials.h"
#include "webfuse/core/status_intern.h"

#include "webfuse/core/jsonrpc/request.h"
#include "webfuse/core/timer/manager.h"
#include "webfuse/core/timer/timer.h"

static int wf_impl_server_protocol_callback(
	struct lws * wsi,
	enum lws_callback_reasons reason,
	void * WF_UNUSED_PARAM(user),
	void * in,
	size_t len)
{
    struct lws_protocols const * ws_protocol = lws_get_protocol(wsi);
    if (NULL == ws_protocol) { return 0; }
    if (ws_protocol->callback != &wf_impl_server_protocol_callback) { return 0; }

    struct wf_server_protocol * protocol = ws_protocol->user;
    wf_timer_manager_check(protocol->timer_manager);
    struct wf_impl_session * session = wf_impl_session_manager_get(&protocol->session_manager, wsi);

    switch (reason)
    {
        case LWS_CALLBACK_PROTOCOL_INIT:
            protocol->is_operational = true;
            break;
		case LWS_CALLBACK_ESTABLISHED:
            session = wf_impl_session_manager_add(
                &protocol->session_manager,
                wsi,
                &protocol->authenticators,
                &protocol->mountpoint_factory,
                protocol->timer_manager,
                protocol->server);

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
            if (NULL != session)
            {
                wf_impl_session_process_filesystem_request(session, wsi);
            }
            break;
        default:
            break;
    }

    return 0;
}

struct wf_server_protocol * wf_impl_server_protocol_create(
    wf_create_mountpoint_fn * create_mountpoint,
    void * create_mountpoint_context)
{
    struct wf_server_protocol * protocol = malloc(sizeof(struct wf_server_protocol));
    struct wf_impl_mountpoint_factory mountpoint_factory;
    wf_impl_mountpoint_factory_init(&mountpoint_factory,
        create_mountpoint, create_mountpoint_context);

    wf_impl_server_protocol_init(protocol, &mountpoint_factory);

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
    lws_protocol->name = WF_PROTOCOL_NAME_ADAPTER_SERVER;
	lws_protocol->callback = &wf_impl_server_protocol_callback;
	lws_protocol->per_session_data_size = 0;
	lws_protocol->user = protocol;
}

static void wf_impl_server_protocol_authenticate(
    struct wf_jsonrpc_request * request,
    char const * WF_UNUSED_PARAM(method_name),
    json_t * params,
    void * WF_UNUSED_PARAM(user_data))
{
    bool result = false;

    json_t * type_holder = json_array_get(params, 0);
    json_t * creds_holder = json_array_get(params, 1);

    if (json_is_string(type_holder) && json_is_object(creds_holder))
    {
        char const * type = json_string_value(type_holder);
        struct wf_credentials creds;
         
        wf_impl_credentials_init(&creds, type, creds_holder);
        struct wf_impl_session * session = wf_jsonrpc_request_get_userdata(request);
        result = wf_impl_session_authenticate(session, &creds);
        
        wf_impl_credentials_cleanup(&creds);
    }


    if (result)
    {
        json_t * result = json_object();
        wf_jsonrpc_respond(request, result);
    }
    else
    {
        wf_jsonrpc_respond_error(request, WF_BAD_ACCESS_DENIED, wf_status_tostring(WF_BAD_ACCESS_DENIED));
    }    
}

static bool wf_impl_server_protocol_check_name(char const * value)
{
    while ('\0' != *value)
    {
        char const c = * value;
        if (!isalpha(c) && !isdigit(c) && ('_' != c))
        {
            return false;
        }
        value++;
    }

    return true;
}

static void wf_impl_server_protocol_add_filesystem(
    struct wf_jsonrpc_request * request,
    char const * WF_UNUSED_PARAM(method_name),
    json_t * params,
    void * WF_UNUSED_PARAM(user_data))
{
    struct wf_impl_session * session = wf_jsonrpc_request_get_userdata(request);
    wf_status status = (session->is_authenticated) ? WF_GOOD : WF_BAD_ACCESS_DENIED;

    char const * name = NULL;
    if (WF_GOOD == status)
    {
        json_t * name_holder = json_array_get(params, 0);
        if (json_is_string(name_holder))
        {
            name = json_string_value(name_holder);
            if (wf_impl_server_protocol_check_name(name))
            {
                bool const success = wf_impl_session_add_filesystem(session, name);
                if (!success)
                {
                    status = WF_BAD;
                }
            }
            else 
            {
                status = WF_BAD_FORMAT;
            }
        }
        else
        {
            status = WF_BAD_FORMAT;
        }
        
    }

    if (WF_GOOD == status)
    {
        json_t * result = json_object();
        json_object_set_new(result, "id", json_string(name));
        wf_jsonrpc_respond(request, result);
    }
    else
    {
        wf_jsonrpc_respond_error(request, status, wf_status_tostring(status));
    }
    

}

void wf_impl_server_protocol_init(
    struct wf_server_protocol * protocol,
    struct wf_impl_mountpoint_factory * mountpoint_factory)
{
    protocol->is_operational = false;

    wf_impl_mountpoint_factory_clone(mountpoint_factory, &protocol->mountpoint_factory);

    protocol->timer_manager = wf_timer_manager_create();
    wf_impl_session_manager_init(&protocol->session_manager);
    wf_impl_authenticators_init(&protocol->authenticators);

    protocol->server = wf_jsonrpc_server_create();
    wf_jsonrpc_server_add(protocol->server, "authenticate", &wf_impl_server_protocol_authenticate, protocol);
    wf_jsonrpc_server_add(protocol->server, "add_filesystem", &wf_impl_server_protocol_add_filesystem, protocol);
}

void wf_impl_server_protocol_cleanup(
    struct wf_server_protocol * protocol)
{
    protocol->is_operational = false;

    wf_jsonrpc_server_dispose(protocol->server);
    wf_timer_manager_dispose(protocol->timer_manager);
    wf_impl_authenticators_cleanup(&protocol->authenticators);
    wf_impl_session_manager_cleanup(&protocol->session_manager);
    wf_impl_mountpoint_factory_cleanup(&protocol->mountpoint_factory);
}

void wf_impl_server_protocol_add_authenticator(
    struct wf_server_protocol * protocol,
    char const * type,
    wf_authenticate_fn * authenticate,
    void * user_data)
{
    wf_impl_authenticators_add(&protocol->authenticators, type, authenticate, user_data);
}
