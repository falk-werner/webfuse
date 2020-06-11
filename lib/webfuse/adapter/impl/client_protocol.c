#include "webfuse/adapter/impl/client_protocol.h"
#include "webfuse/adapter/client_callback.h"
#include "webfuse/core/protocol_names.h"
#include "webfuse/core/util.h"

#include <stddef.h>
#include <libwebsockets.h>

static int wf_impl_client_protocol_lws_callback(
	struct lws * WF_UNUSED_PARAM(wsi),
	enum lws_callback_reasons WF_UNUSED_PARAM(reason),
	void * WF_UNUSED_PARAM(user),
	void * WF_UNUSED_PARAM(in),
	size_t WF_UNUSED_PARAM(len))
{
    return 0;
}

void
wf_impl_client_protocol_init(
    struct wf_client_protocol * protocol,
    wf_client_callback_fn * callback,
    void * user_data)
{
    protocol->callback = callback;
    protocol->user_data = user_data;
    protocol->callback(protocol->user_data, WF_CLIENT_INIT, NULL);
}

void
wf_impl_client_protocol_cleanup(
    struct wf_client_protocol * protocol)
{
    protocol->callback(protocol->user_data, WF_CLIENT_CLEANUP, NULL);
}

void
wf_impl_client_protocol_callback(
    struct wf_client_protocol * protocol,
    int reason,
    void * arg)
{
    protocol->callback(protocol->user_data, reason, arg);
}

void
wf_impl_client_protocol_init_lws(
    struct wf_client_protocol * protocol,
    struct lws_protocols * lws_protocol)
{
    lws_protocol->name = WF_PROTOCOL_NAME_ADAPTER_CLIENT;
    lws_protocol->callback = &wf_impl_client_protocol_lws_callback;
    lws_protocol->per_session_data_size = 0;
    lws_protocol->user = protocol;
}
