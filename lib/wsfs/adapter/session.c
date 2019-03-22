#include "wsfs/adapter/session.h"
#include <stddef.h>

void wsfs_session_init(
    struct wsfs_session * session,
    struct lws * wsi,
    bool is_authenticated)
 {
     session->wsi = wsi;
     session->is_authenticated = is_authenticated;
     wsfs_message_queue_init(&session->queue);
 }

void wsfs_session_cleanup(
    struct wsfs_session * session)
{
    wsfs_message_queue_cleanup(&session->queue);
    session->is_authenticated = false;
    session->wsi = NULL;
}