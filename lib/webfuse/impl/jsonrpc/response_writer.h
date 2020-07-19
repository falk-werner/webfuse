#ifndef WF_IMPL_JSONRPC_RESPONSE_WRITER_H
#define WF_IMPL_JSONRPC_RESPONSE_WRITER_H

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_jsonrpc_response_writer;
struct wf_messge;

extern struct wf_jsonrpc_response_writer *
wf_impl_jsonrpc_response_writer_create(int id);

extern void
wf_impl_jsonrpc_response_writer_dispose(
    struct wf_jsonrpc_response_writer * writer);

extern struct wf_message *
wf_impl_jsonrpc_response_writer_take_message(
    struct wf_jsonrpc_response_writer * writer);

extern void
wf_impl_jsonrpc_response_add_int(
    struct wf_jsonrpc_response_writer * writer,
    char const * key,
    int value);

extern void
wf_impl_jsonrpc_response_add_string(
    struct wf_jsonrpc_response_writer * writer,
    char const * key,
    char const * value);


#ifdef __cplusplus
}
#endif

#endif
