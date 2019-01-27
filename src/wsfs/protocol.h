#ifndef _WSFS_PROTOCOL_H
#define _WSFS_PROTOCOL_H

#include <stdbool.h>
#include <libwebsockets.h>
#include <pthread.h>

struct wsfs_protocol;
struct wsfs_server;

extern struct wsfs_protocol * wsfs_protocol_create(
	struct wsfs_server * server);

extern void wsfs_protocol_dispose(
	struct wsfs_protocol * protocol);

extern void wsfs_protocol_check(
	struct wsfs_protocol * protocol);

extern void wsfs_protocol_init_lws(
	struct wsfs_protocol * protocl,
	struct lws_protocols * lws_protocol);

extern char * wsfs_protocol_message_create(
	size_t size);

extern void wsfs_protocol_message_dispose(
	char * message);

extern bool wsfs_protocol_send(
	char * message,
	size_t length,
	void * user_data);


#endif

