#ifndef WF_ADAPTER_IMPL_OPERATION_CLOSE_H
#define WF_ADAPTER_IMPL_OPERATION_CLOSE_H

#include "webfuse/adapter/impl/fuse_wrapper.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern void wf_impl_operation_close(
	fuse_req_t request,
	fuse_ino_t inode,
	struct fuse_file_info * file_info);

#ifdef __cplusplus
}
#endif

#endif
