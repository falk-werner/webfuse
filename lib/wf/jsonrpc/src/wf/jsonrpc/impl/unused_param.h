#ifndef WF_JSONRPC_UTIL_H
#define WF_JSONRPC_UTIL_H

#ifdef __GNUC__
#define WF_JSONRPC_UNUSED_PARAM(param) param __attribute__((unused))
#else
#define WF_JSONRPC_UNUSED_PARAM(param)
#endif

#endif
