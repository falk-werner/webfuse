#ifndef WF_ADAPTER_IMPL_OPERATION_OPEN_H
#define WF_ADAPTER_IMPL_OPERATION_OPEN_H

#include "webfuse/adapter/impl/fuse_wrapper.h"
#include <jansson.h>

#ifdef __cplusplus
extern "C"
{
#endif

extern void wf_impl_operation_open(
	fuse_req_t request,
	fuse_ino_t inode,
	struct fuse_file_info * file_info);

extern void wf_impl_operation_open_finished(
	void * user_data,
	json_t const * result,
	json_t const * error);

#ifdef __cplusplus
}
#endif

#endif
