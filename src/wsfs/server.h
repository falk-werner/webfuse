#ifndef _WSFS_SERVER_H
#define _WSFS_SERVER_H

#ifndef __cplusplus
#include <stddef.h>
#else
#include <cstddef>
using std::size_t;
#endif

struct wsfs_server;
struct wsfs_jsonrpc;

struct wsfs_server_config
{
	char * document_root;
	char * key_path;
	char * cert_path;
	char * vhost_name;
	int port;
};

#ifdef __cplusplus
extern "C" {
#endif

extern void wsfs_server_config_clear(
	struct wsfs_server_config * config);

extern struct wsfs_server * wsfs_server_create(
	struct wsfs_server_config * config);

extern void wsfs_server_dispose(
	struct wsfs_server * server);

extern struct wsfs_jsonrpc * wsfs_server_get_jsonrpc_service(
	struct wsfs_server * server);

extern void wsfs_server_start(
	struct wsfs_server * server);

extern void wsfs_server_wakeup(
	struct wsfs_server * server);

extern void wsfs_server_handle_message(
	struct wsfs_server * server,
	char const * message,
	size_t length);

#ifdef __cplusplus
}
#endif

#endif

