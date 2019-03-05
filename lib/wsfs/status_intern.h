#ifndef WSFS_STATUS_INTERN_H
#define WSFS_STATUS_INTERN_H

#include "wsfs/status.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int wsfs_status_to_rc(wsfs_status status);

extern char const * wsfs_status_tostring(wsfs_status status);

#ifdef __cplusplus
}
#endif

#endif

