#ifndef _WSFS_STATUS_H
#define _WSFS_STATUS_H

#define WSFS_GOOD 0
#define WSFS_BAD  1

#define WSFS_BAD_NOTIMPLEMENTED 2
#define WSFS_BAD_TIMEOUT        3
#define WSFS_BAD_BUSY           4
#define WSFS_BAD_FORMAT         5

#define WSFS_BAD_NOENTRY 101
#define WSFS_BAD_NOACCESS 102

typedef int wsfs_status;

#ifdef __cplusplus
extern "C" {
#endif

extern int wsfs_status_to_rc(wsfs_status status);

#ifdef __cplusplus
}
#endif

#endif

