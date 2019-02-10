#ifndef _WSFS_SERVER_PROTOCOL_H
#define _WSFS_SERVER_PROTOCOL_H

#include "wsfs/api.h"

struct wsfs_server_protocol;
struct lws_protocols;

#ifdef __cplusplus
extern "C"
{
#endif

extern WSFS_API struct wsfs_server_protocol * wsfs_server_protocol_create(
    char * mount_point);

extern WSFS_API void wsfs_server_protocol_dispose(
    struct wsfs_server_protocol * protocol);

extern WSFS_API void wsfs_server_protocol_init_lws(
    struct wsfs_server_protocol * protocol,
    struct lws_protocols * lws_protocol);

#ifdef __cplusplus
}
#endif

#endif
