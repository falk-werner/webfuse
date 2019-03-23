#include "wsfs/adapter/server.h"

#include <stdlib.h>
#include <stdbool.h>
#include <libwebsockets.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "wsfs/adapter/server_config_intern.h"
#include "wsfs/adapter/server_protocol_intern.h"

#define WSFS_DISABLE_LWS_LOG 0
#define WSFS_SERVER_PROTOCOL_COUNT 3
#define WSFS_SERVER_TIMEOUT (1 * 1000)

struct wsfs_server
{
    struct wsfs_server_config config;
    struct wsfs_server_protocol protocol;
    struct lws_protocols ws_protocols[WSFS_SERVER_PROTOCOL_COUNT];
    struct lws_context * context;
    volatile bool shutdown_requested;
	struct lws_http_mount mount;
	struct lws_context_creation_info info;
};

static bool wsfs_server_tls_enabled(
	struct wsfs_server * server)
{
	return ((server->config.key_path != NULL) && (server->config.cert_path != NULL));
}

static struct lws_context * wsfs_server_context_create(
    struct wsfs_server * server)
{
	lws_set_log_level(WSFS_DISABLE_LWS_LOG, NULL);

    memset(server->ws_protocols, 0, sizeof(struct lws_protocols) * WSFS_SERVER_PROTOCOL_COUNT);
    server->ws_protocols[0].name = "http";
    server->ws_protocols[0].callback = lws_callback_http_dummy;
    server->ws_protocols[1].name = "fs";
    wsfs_server_protocol_init_lws(&server->protocol, &server->ws_protocols[1]);

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

	if (wsfs_server_tls_enabled(server))
	{
		server->info.options |= LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
		server->info.ssl_cert_filepath = server->config.cert_path;
		server->info.ssl_private_key_filepath = server->config.key_path;
	}

	struct lws_context * const context = lws_create_context(&server->info);
    return context;

}

static bool wsfs_server_check_mountpoint(
	 struct wsfs_server_config * config)
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

struct wsfs_server * wsfs_server_create(
    struct wsfs_server_config * config)
{
	struct wsfs_server * server = NULL;
	
	if (wsfs_server_check_mountpoint(config))
	{
		server = malloc(sizeof(struct wsfs_server));
		if (NULL != server)
		{
			if (wsfs_server_protocol_init(&server->protocol, config->mount_point))
			{
				server->shutdown_requested = false;
				wsfs_server_config_clone(config, &server->config);
				wsfs_authenticators_move(&server->config.authenticators, &server->protocol.authenticators);				
				server->context = wsfs_server_context_create(server);
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

