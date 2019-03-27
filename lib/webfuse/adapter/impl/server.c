#include "webfuse/adapter/server.h"

#include <stdlib.h>
#include <stdbool.h>
#include <libwebsockets.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "webfuse/adapter/impl/server_config.h"
#include "webfuse/adapter/impl/server_protocol.h"

#define WF_DISABLE_LWS_LOG 0
#define WF_SERVER_PROTOCOL_COUNT 3
#define WF_SERVER_TIMEOUT (1 * 1000)

struct wf_server
{
    struct wf_server_config config;
    struct wf_server_protocol protocol;
    struct lws_protocols ws_protocols[WF_SERVER_PROTOCOL_COUNT];
    struct lws_context * context;
    volatile bool shutdown_requested;
	struct lws_http_mount mount;
	struct lws_context_creation_info info;
};

static bool wf_impl_server_tls_enabled(
	struct wf_server * server)
{
	return ((server->config.key_path != NULL) && (server->config.cert_path != NULL));
}

static struct lws_context * wf_impl_server_context_create(
    struct wf_server * server)
{
	lws_set_log_level(WF_DISABLE_LWS_LOG, NULL);

    memset(server->ws_protocols, 0, sizeof(struct lws_protocols) * WF_SERVER_PROTOCOL_COUNT);
    server->ws_protocols[0].name = "http";
    server->ws_protocols[0].callback = lws_callback_http_dummy;
    server->ws_protocols[1].name = "fs";
    wf_impl_server_protocol_init_lws(&server->protocol, &server->ws_protocols[1]);

	memset(&server->mount, 0, sizeof(struct lws_http_mount));
	server->mount.mount_next = NULL,
	server->mount.mountpoint = "/",
	server->mount.origin = server->config.document_root,
	server->mount.def = "index.html",
	server->mount.origin_protocol = LWSMPRO_FILE,
	server->mount.mountpoint_len = 1,

	memset(&server->info, 0, sizeof(struct lws_context_creation_info));
	server->info.port = server->config.port;
	server->info.mounts = &server->mount;
	server->info.protocols = server->ws_protocols;
	server->info.vhost_name = server->config.vhost_name;
	server->info.ws_ping_pong_interval = 10;
	server->info.options = LWS_SERVER_OPTION_HTTP_HEADERS_SECURITY_BEST_PRACTICES_ENFORCE;

	if (NULL == server->config.document_root)
	{
		// disable http
		server->info.protocols = &server->ws_protocols[1];
		server->info.mounts = NULL;
	}

	if (wf_impl_server_tls_enabled(server))
	{
		server->info.options |= LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
		server->info.ssl_cert_filepath = server->config.cert_path;
		server->info.ssl_private_key_filepath = server->config.key_path;
	}

	struct lws_context * const context = lws_create_context(&server->info);
    return context;

}

static bool wf_impl_server_check_mountpoint(
	 struct wf_server_config * config)
{
	bool result = false;

	if (NULL != config->mount_point)
	{
		struct stat info;
		int const rc = stat(config->mount_point, &info);
		result = ((0 == rc) && (S_ISDIR(info.st_mode)));

		if (!result)
		{
			result = (0 == mkdir(config->mount_point, 0755));
		}
	}

	return result;
}

struct wf_server * wf_impl_server_create(
    struct wf_server_config * config)
{
	struct wf_server * server = NULL;
	
	if (wf_impl_server_check_mountpoint(config))
	{
		server = malloc(sizeof(struct wf_server));
		if (NULL != server)
		{
			if (wf_impl_server_protocol_init(&server->protocol, config->mount_point))
			{
				server->shutdown_requested = false;
				wf_impl_server_config_clone(config, &server->config);
				wf_impl_authenticators_move(&server->config.authenticators, &server->protocol.authenticators);				
				server->context = wf_impl_server_context_create(server);
			}
			else
			{
				free(server);
				server = NULL;
			}
		}   
	}

    return server; 
}

void wf_impl_server_dispose(
    struct wf_server * server)
{
    lws_context_destroy(server->context);
    wf_impl_server_protocol_cleanup(&server->protocol);
    wf_impl_server_config_cleanup(&server->config);
    free(server);   
}

void wf_impl_server_run(
    struct wf_server * server)
{
    int n = 0;
    while ((0 <= n) && (!server->shutdown_requested))
    {
        n = lws_service(server->context, WF_SERVER_TIMEOUT);
    }
}

void wf_impl_server_shutdown(
    struct wf_server * server)
{
    server->shutdown_requested = true;
}

