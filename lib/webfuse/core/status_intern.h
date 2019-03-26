#ifndef WF_STATUS_INTERN_H
#define WF_STATUS_INTERN_H

#include "webfuse/core/status.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int wf_status_to_rc(wf_status status);

extern char const * wf_status_tostring(wf_status status);

#ifdef __cplusplus
}
#endif

#endif

