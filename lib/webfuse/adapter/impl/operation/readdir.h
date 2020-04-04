#ifndef WF_ADAPTER_IMPL_OPERATION_READDIR_H
#define WF_ADAPTER_IMPL_OPERATION_READDIR_H

#include "webfuse/adapter/impl/fuse_wrapper.h"
#include <jansson.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_impl_operation_readdir_context
{
	fuse_req_t request;
	size_t size;
	off_t offset;
};

extern void wf_impl_operation_readdir (
	fuse_req_t request,
	fuse_ino_t inode,
	size_t size,
	off_t offset,
	struct fuse_file_info *file_info);

extern void wf_impl_operation_readdir_finished(
	void * user_data,
	json_t const * result,
	json_t const * error);

#ifdef __cplusplus
}
#endif

#endif
