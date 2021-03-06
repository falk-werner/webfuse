#ifndef WF_ADAPTER_IMPL_OPERATION_GETATTR_H
#define WF_ADAPTER_IMPL_OPERATION_GETATTR_H

#include "webfuse/impl/fuse_wrapper.h"

#include <sys/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_jsonrpc_error;
struct wf_json;

struct wf_impl_operation_getattr_context
{
	fuse_req_t request;
	fuse_ino_t inode;
	double timeout;
	uid_t uid;
	gid_t gid;
};

extern void wf_impl_operation_getattr_finished(
	void * user_data,
	struct wf_json const * result,
	struct wf_jsonrpc_error const * error);

extern void wf_impl_operation_getattr (
		fuse_req_t request,
		fuse_ino_t inode,
		struct fuse_file_info *file_info);

#ifdef __cplusplus
}
#endif

#endif
