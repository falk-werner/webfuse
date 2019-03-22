#ifndef WSFS_ADAPTER_SESSION_H
#define WSFS_ADAPTER_SESSION_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#include "wsfs/message_queue.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct lws;

struct wsfs_session
{
    struct lws * wsi;
    bool is_authenticated;
    struct wsfs_message_queue queue;
};

extern void wsfs_session_init(
    struct wsfs_session * session,
    struct lws * wsi,
    bool is_authenticated);

extern void wsfs_session_cleanup(
    struct wsfs_session * session);

#ifdef __cplusplus
}
#endif


#endif
