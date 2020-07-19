#ifndef WF_ADAPTER_IMPL_OPERATION_READ_H
#define WF_ADAPTER_IMPL_OPERATION_READ_H

#include "webfuse/impl/fuse_wrapper.h"
#include "webfuse/status.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_jsonrpc_error;
struct wf_json;

extern void wf_impl_operation_read(
	fuse_req_t request,
	fuse_ino_t ino, size_t size, off_t off,
			struct fuse_file_info *fi);

extern char * wf_impl_operation_read_transform(
	char * data,
	size_t data_size,
	char const * format,
	size_t count,
	wf_status * status);

extern void wf_impl_operation_read_finished(
	void * user_data, 
	struct wf_json const * result,
	struct wf_jsonrpc_error const * error);


#ifdef __cplusplus
}
#endif

#endif
