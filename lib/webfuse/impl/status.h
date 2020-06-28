#ifndef WF_IMPL_STATUS_H
#define WF_IMPL_STATUS_H

#include "webfuse/status.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int wf_impl_status_to_rc(wf_status status);

extern char const * wf_impl_status_tostring(wf_status status);

#ifdef __cplusplus
}
#endif

#endif

