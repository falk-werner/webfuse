#ifndef WSFS_ADAPTER_IMPL_SESSION_H
#define WSFS_ADAPTER_IMPL_SESSION_H

#ifndef __cplusplus
#include <stdbool.h>
#include <stddef.h>
#else
#include <cstddef>
using std::size_t;
#endif

#include "wsfs/message_queue.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct lws;
struct wsfs_message;
struct wsfs_credentials;
struct wsfs_authenticators;
struct wsfs_jsonrpc_server;

struct wsfs_session
{
    struct lws * wsi;
    bool is_authenticated;
    struct wsfs_message_queue queue;
    struct wsfs_authenticators * authenticators;
    struct wsfs_jsonrpc_server * rpc;
};

extern void wsfs_session_init(
    struct wsfs_session * session,
    struct lws * wsi,
   struct wsfs_authenticators * authenticators,
   struct wsfs_jsonrpc_server * rpc);

extern void wsfs_session_authenticate(
    struct wsfs_session * session,
    struct wsfs_credentials * creds);

extern bool wsfs_session_send(
    struct wsfs_session * session,
    struct wsfs_message * message);

extern void wsfs_session_receive(
    struct wsfs_session * session,
    char const * data,
    size_t length);

extern void wsfs_session_onwritable(
    struct wsfs_session * session);

extern void wsfs_session_cleanup(
    struct wsfs_session * session);

#ifdef __cplusplus
}
#endif


#endif
