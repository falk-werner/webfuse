#ifndef JSONRPC_UTIL_H
#define JSONRPC_UTIL_H

#ifdef __GNUC__
#define JSONRPC_UNUSED_PARAM(param) param __attribute__((unused))
#else
#define JSONRPC_UNUSED_PARAM(param)
#endif

#endif
