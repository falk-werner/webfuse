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
struct authenticators;
struct jsonrpc_server;

struct session
{
    struct lws * wsi;
    bool is_authenticated;
    struct wsfs_message_queue queue;
    struct authenticators * authenticators;
    struct jsonrpc_server * rpc;
};

extern void session_init(
    struct session * session,
    struct lws * wsi,
   struct authenticators * authenticators,
   struct jsonrpc_server * rpc);

extern void session_authenticate(
    struct session * session,
    struct wsfs_credentials * creds);

extern bool session_send(
    struct session * session,
    struct wsfs_message * message);

extern void session_receive(
    struct session * session,
    char const * data,
    size_t length);

extern void session_onwritable(
    struct session * session);

extern void session_cleanup(
    struct session * session);

#ifdef __cplusplus
}
#endif


#endif
