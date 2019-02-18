#include "wsfsp/client_protocol_intern.h"

#include <stdlib.h>
#include <string.h>

#include <libwebsockets.h>

#include "wsfsp/provider_default.h"
#include "wsfs/util.h"

static int wsfsp_client_protocol_callback(
	struct lws * WSFS_UNUSED_PARAM(wsi),
	enum lws_callback_reasons WSFS_UNUSED_PARAM(reason),
	void * WSFS_UNUSED_PARAM(user),
	void * WSFS_UNUSED_PARAM(in),
	size_t WSFS_UNUSED_PARAM(len))
{
    return 0;
}


void wsfsp_client_protocol_init(
    struct wsfsp_client_protocol * protocol,
    struct wsfsp_provider const * provider,
    void * user_data)
{
    protocol->user_data = user_data;
    protocol->provider.lookup = (NULL != provider->lookup) ? provider->lookup : &wsfsp_lookup_default;
    protocol->provider.getattr = (NULL != provider->getattr) ? provider->getattr : &wsfsp_getattr_default;
    protocol->provider.readdir = (NULL != provider->readdir) ? provider->readdir : &wsfsp_readdir_default;
    protocol->provider.open = (NULL != provider->open) ? provider->open : &wsfsp_open_default;
    protocol->provider.close = (NULL != provider->close) ? provider->close : &wsfsp_close_default;
    protocol->provider.read = (NULL != provider->read) ? provider->read : &wsfsp_read_default;
    protocol->provider.connected = (NULL != provider->connected) ? provider->connected : &wsfsp_connected_default;
    protocol->provider.disconnected = (NULL != provider->disconnected) ? provider->disconnected : &wsfsp_disconnected_default;
    protocol->provider.ontimer = (NULL != provider->ontimer) ? provider->ontimer : &wsfsp_ontimer_default;
}

void wsfsp_client_protocol_cleanup(
    struct wsfsp_client_protocol * protocol)
{
    (void) protocol;
}

struct wsfsp_client_protocol * wsfsp_client_protocol_create(
    struct wsfsp_provider const * provider,
    void * user_data)
{
    struct wsfsp_client_protocol * protocol = malloc(sizeof(struct wsfsp_client_protocol));
    if (NULL != protocol)
    {
        wsfsp_client_protocol_init(protocol, provider, user_data);
    }

    return protocol;
}

void wsfsp_client_protocol_dispose(
    struct wsfsp_client_protocol * protocol)
{
    wsfsp_client_protocol_cleanup(protocol);
    free(protocol);
}

void wsfsp_client_protocol_init_lws(
    struct wsfsp_client_protocol * protocol,
    struct lws_protocols * lws_protocol)
{
	lws_protocol->callback = &wsfsp_client_protocol_callback;
	lws_protocol->per_session_data_size = 0;
	lws_protocol->user = protocol;
}
