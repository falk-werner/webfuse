#ifndef WF_ADAPTER_IMPL_OPERATION_READ_H
#define WF_ADAPTER_IMPL_OPERATION_READ_H

#include "webfuse/adapter/impl/fuse_wrapper.h"
#include "webfuse/core/status.h"

#include <jansson.h>

#ifdef __cplusplus
extern "C"
{
#endif

extern void wf_impl_operation_read(
	fuse_req_t request,
	fuse_ino_t ino, size_t size, off_t off,
			struct fuse_file_info *fi);

extern char * wf_impl_fill_buffer(
	char const * data,
	size_t data_size,
	char const * format,
	size_t count,
	wf_status * status);

extern void wf_impl_operation_read_finished(
	void * user_data, 
	json_t const * result,
	json_t const * error);


#ifdef __cplusplus
}
#endif

#endif
