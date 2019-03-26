#ifndef WSFS_ADAPTER_IMPL_SESSION_H
#define WSFS_ADAPTER_IMPL_SESSION_H

#ifndef __cplusplus
#include <stdbool.h>
#include <stddef.h>
#else
#include <cstddef>
using std::size_t;
#endif

#include "wsfs/core/message_queue.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct lws;
struct wsfs_message;
struct wsfs_credentials;
struct wsfs_impl_authenticators;
struct wsfs_impl_jsonrpc_server;

struct wsfs_impl_session
{
    struct lws * wsi;
    bool is_authenticated;
    struct wsfs_message_queue queue;
    struct wsfs_impl_authenticators * authenticators;
    struct wsfs_impl_jsonrpc_server * rpc;
};

extern void wsfs_impl_session_init(
    struct wsfs_impl_session * session,
    struct lws * wsi,
   struct wsfs_impl_authenticators * authenticators,
   struct wsfs_impl_jsonrpc_server * rpc);

extern void wsfs_impl_session_authenticate(
    struct wsfs_impl_session * session,
    struct wsfs_credentials * creds);

extern bool wsfs_impl_session_send(
    struct wsfs_impl_session * session,
    struct wsfs_message * message);

extern void wsfs_impl_session_receive(
    struct wsfs_impl_session * session,
    char const * data,
    size_t length);

extern void wsfs_impl_session_onwritable(
    struct wsfs_impl_session * session);

extern void wsfs_impl_session_cleanup(
    struct wsfs_impl_session * session);

#ifdef __cplusplus
}
#endif


#endif
