#include "wsfs/server.h"

#include <stdlib.h>
#include <stdbool.h>
#include <libwebsockets.h>

#include "wsfs/server_config.h"
#include "wsfs/server_protocol_intern.h"

#define WSFS_SERVER_PROTOCOL_COUNT 3
#define WSFS_SERVER_TIMEOUT (1 * 1000)

struct wsfs_server
{
    struct wsfs_server_config config;
    struct wsfs_server_protocol protocol;
    struct lws_protocols ws_protocols[WSFS_SERVER_PROTOCOL_COUNT];
    struct lws_context * context;
    volatile bool shutdown_requested;
};

static bool wsfs_server_tls_enabled(
	struct wsfs_server * server)
{
	return ((server->config.key_path != NULL) && (server->config.cert_path != NULL));
}

static struct lws_context * wsfs_server_context_create(
    struct wsfs_server * server)
{
    memset(server->ws_protocols, 0, sizeof(struct lws_protocols) * WSFS_SERVER_PROTOCOL_COUNT);
    server->ws_protocols[0].name = "http";
    server->ws_protocols[0].callback = lws_callback_http_dummy;
    server->ws_protocols[1].name = "fs";
    wsfs_server_protocol_init_lws(&server->protocol, &server->ws_protocols[1]);

	struct lws_http_mount mount = 
	{
		.mount_next = NULL,
		.mountpoint = "/",
		.origin = server->config.document_root,
		.def = "index.html",
		.protocol = NULL,
		.cgienv = NULL,
		.extra_mimetypes = NULL,
		.interpret = NULL,
		.cgi_timeout = 0,
		.cache_max_age = 0,
		.auth_mask = 0,
		.cache_reusable = 0,
		.cache_intermediaries = 0,
		.origin_protocol = LWSMPRO_FILE,
		.mountpoint_len = 1,
		.basic_auth_login_file = NULL
	};

	struct lws_context_creation_info info;
	memset(&info, 0, sizeof(info));
	info.port = server->config.port;
	info.mounts = &mount;
	info.protocols = server->ws_protocols;
	info.vhost_name = server->config.vhost_name;
	info.ws_ping_pong_interval = 10;
	info.options = LWS_SERVER_OPTION_HTTP_HEADERS_SECURITY_BEST_PRACTICES_ENFORCE;

	if (NULL == server->config.document_root)
	{
		// disable http
		info.protocols = &server->ws_protocols[1];
		info.mounts = NULL;
	}

	if (wsfs_server_tls_enabled(server))
	{
		info.options |= LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
		info.ssl_cert_filepath = server->config.cert_path;
		info.ssl_private_key_filepath = server->config.key_path;
	}

	struct lws_context * const context = lws_create_context(&info);
    return context;

}

struct wsfs_server * wsfs_server_create(
    struct wsfs_server_config * config)
{
    struct wsfs_server * server = malloc(sizeof(struct wsfs_server));
    if (NULL != server)
    {
        server->shutdown_requested = false;
        wsfs_server_config_clone(config, &server->config);
        wsfs_server_protocol_init(&server->protocol, config->mount_point);
        server->context = wsfs_server_context_create(server);
    }   

    return server; 
}

void wsfs_server_dispose(
    struct wsfs_server * server)
{
    lws_context_destroy(server->context);
    wsfs_server_protocol_cleanup(&server->protocol);
    wsfs_server_config_cleanup(&server->config);
    free(server);   
}

void wsfs_server_run(
    struct wsfs_server * server)
{
    int n = 0;
    while ((0 <= n) && (!server->shutdown_requested))
    {
        n = lws_service(server->context, WSFS_SERVER_TIMEOUT);
    }
}

void wsfs_server_shutdown(
    struct wsfs_server * server)
{
    server->shutdown_requested = true;
}

