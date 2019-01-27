#include "wsfs/server.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>

#include <unistd.h>
#include <pthread.h>

#include <libwebsockets.h>

#include "wsfs/util.h"
#include "wsfs/protocol.h"
#include "wsfs/jsonrpc.h"

#define WSFS_SERVICE_INTERVAL (1 * 1000)

struct wsfs_server
{
	pthread_t thread;
	bool shutdown_flag;
	pthread_mutex_t lock;
	struct wsfs_server_config config;
	struct wsfs_protocol * protocol;
	struct wsfs_jsonrpc * rpc;
};

static char * wsfs_strdup(char const * value)
{
	char * result = NULL;
	if (NULL != value)
	{
		result = strdup(value);
	}

	return result;
}


static bool wsfs_server_isshutdown(
	struct wsfs_server * server)
{
	bool result;

	pthread_mutex_lock(&server->lock);
	result = server->shutdown_flag;
	pthread_mutex_unlock(&server->lock);

	return result;
}

static bool wsfs_server_tls_enabled(
	struct wsfs_server * server)
{
	return ((server->config.key_path != NULL) && (server->config.cert_path != NULL));
}

static void wsfs_server_request_shutdown(
	struct wsfs_server * server)
{
	pthread_mutex_lock(&server->lock);
	server->shutdown_flag = true;
	pthread_mutex_unlock(&server->lock);
}

static void wsfs_ignore_signal(int WSFS_UNUSED_PARAM(signal_id))
{
}

static void* wsfs_server_run(void * arg)
{
	struct wsfs_server * const server = arg;

	struct lws_protocols protocols[] = 
	{
		{"http", lws_callback_http_dummy, 0, 0, 0, NULL, 0},
		{ "fs", NULL, 0 , 0, 0, NULL, 0},
		{ NULL, NULL, 0 , 0, 0, NULL, 0}
	};

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

	wsfs_protocol_init_lws(server->protocol, &protocols[1]);

	struct lws_context_creation_info info;
	memset(&info, 0, sizeof(info));
	info.port = server->config.port;
	info.mounts = &mount;
	info.protocols = protocols;
	info.vhost_name = server->config.vhost_name;
	info.ws_ping_pong_interval = 10;
	info.options = LWS_SERVER_OPTION_HTTP_HEADERS_SECURITY_BEST_PRACTICES_ENFORCE;

	if (NULL == server->config.document_root)
	{
		// disable http
		info.protocols = &(protocols[1]);
		info.mounts = NULL;
	}

	if (wsfs_server_tls_enabled(server))
	{
		info.options |= LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
		info.ssl_cert_filepath = server->config.cert_path;
		info.ssl_private_key_filepath = server->config.key_path;
	}

	struct lws_context * context = lws_create_context(&info);
	if (NULL == context)
	{
		fprintf(stderr, "error: unable to start websocket server\n");
		return NULL;
	}

	int n = 0;
	while ((0 <= n) && (!wsfs_server_isshutdown(server)))
	{
		wsfs_protocol_check(server->protocol);
		n = lws_service(context, WSFS_SERVICE_INTERVAL);
	}

	lws_context_destroy(context);
	return NULL;
}

static void wsfs_server_join(struct wsfs_server * server)
{	
	wsfs_server_request_shutdown(server);
	pthread_join(server->thread, NULL);
}

struct wsfs_server * wsfs_server_create(
	struct wsfs_server_config * config)
{
	signal(SIGUSR1, &wsfs_ignore_signal);

	struct wsfs_server * server = malloc(sizeof(struct wsfs_server));
	if (NULL != server)
	{
		pthread_mutex_init(&server->lock, NULL);
		server->shutdown_flag = false;
		server->config.document_root = wsfs_strdup(config->document_root);
		server->config.cert_path = wsfs_strdup(config->cert_path);
		server->config.key_path = wsfs_strdup(config->key_path);
		server->config.vhost_name = wsfs_strdup(config->vhost_name);
		server->config.port = config->port;

		server->rpc = wsfs_jsonrpc_create(&wsfs_protocol_message_create, &wsfs_protocol_send, NULL);
		server->protocol = wsfs_protocol_create(server);
		wsfs_jsonrpc_set_user_data(server->rpc, server->protocol);
	}

	return server;
}

void wsfs_server_dispose(
	struct wsfs_server * server)
{
	wsfs_server_join(server);
	
	wsfs_jsonrpc_dispose(server->rpc);
	wsfs_protocol_dispose(server->protocol);
	wsfs_server_config_clear(&server->config);
	pthread_mutex_destroy(&server->lock);
	free(server);
}

void wsfs_server_start(
	struct wsfs_server * server)
{
	pthread_create(&server->thread, NULL, &wsfs_server_run, server);	
}

void wsfs_server_config_clear(struct wsfs_server_config * config)
{
	free(config->document_root);
	free(config->cert_path);
	free(config->key_path);
	free(config->vhost_name);
}

struct wsfs_jsonrpc * wsfs_server_get_jsonrpc_service(
	struct wsfs_server * server)
{
	return server->rpc;
}

void wsfs_server_wakeup(
	struct wsfs_server * server)
{
	pthread_kill(server->thread, SIGUSR1);
}

void wsfs_server_handle_message(
	struct wsfs_server * server,
	char const * message,
	size_t length)
{
	wsfs_jsonrpc_on_message(message, length, server->rpc);
}

