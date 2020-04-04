#ifndef WF_ADAPTER_IMPL_OPERATION_LOOKUP_H
#define WF_ADAPTER_IMPL_OPERATION_LOOKUP_H

#include "webfuse/adapter/impl/fuse_wrapper.h"

#include <jansson.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_impl_operation_lookup_context
{
	fuse_req_t request;
	double timeout;
	uid_t uid;
	gid_t gid;
};

extern void wf_impl_operation_lookup_finished(
	void * user_data,
	json_t const * result,
	json_t const * error);

extern void wf_impl_operation_lookup (
	fuse_req_t req, 
	fuse_ino_t parent, 
	char const * name);

#ifdef __cplusplus
}
#endif

#endif
