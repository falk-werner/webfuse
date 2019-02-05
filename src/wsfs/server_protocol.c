#include "wsfs/server_protocol_intern.h"

#include <stdlib.h>
#include <libwebsockets.h>

#include "wsfs/filesystem.h"
#include "wsfs/util.h"

static int wsfs_server_protocol_callback(
	struct lws * wsi,
	enum lws_callback_reasons reason,
	void * WSFS_UNUSED_PARAM(user),
	void * WSFS_UNUSED_PARAM(in),
	size_t WSFS_UNUSED_PARAM(len))
{
    struct lws_protocols const * ws_protocol = lws_get_protocol(wsi);
    struct wsfs_server_protocol * protocol = ws_protocol->user;

    switch (reason)
    {
        case LWS_CALLBACK_PROTOCOL_INIT:
        {            
            lws_sock_file_fd_type fd;
            fd.filefd = wsfs_filesystem_get_fd(&protocol->filesystem);
            if (!lws_adopt_descriptor_vhost(lws_get_vhost(wsi), LWS_ADOPT_RAW_FILE_DESC, fd, ws_protocol->name, NULL))
            {
                puts("error: unable to adopt fd");
            }
        }
        break;
        case LWS_CALLBACK_RAW_RX_FILE:
        {
            puts("fd readable");
            wsfs_filesystem_process_request(&protocol->filesystem);
        }
        break;
        default:
        break;
    }
    return 0;
}

struct wsfs_server_protocol * wsfs_server_protocol_create(
    char * mount_point)
{
    struct wsfs_server_protocol * protocol = malloc(sizeof(struct wsfs_server_protocol));
    if (NULL != protocol)
    {
        wsfs_server_protocol_init(protocol, mount_point);
    }

    return protocol;
}

void wsfs_server_protocol_dispose(
    struct wsfs_server_protocol * protocol)
{
    wsfs_server_protocol_cleanup(protocol);
    free(protocol);
}

void wsfs_server_protocol_init_lws(
    struct wsfs_server_protocol * protocol,
    struct lws_protocols * lws_protocol)
{
	lws_protocol->callback = &wsfs_server_protocol_callback;
	lws_protocol->per_session_data_size = 0;
	lws_protocol->user = protocol;
}

void wsfs_server_protocol_init(
    struct wsfs_server_protocol * protocol,
    char * mount_point)
{
    wsfs_filesystem_init(&protocol->filesystem, mount_point);
}

void wsfs_server_protocol_cleanup(
    struct wsfs_server_protocol * protocol)
{
    wsfs_filesystem_cleanup(&protocol->filesystem);
}
