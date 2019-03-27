#ifndef WF_STATUS_H
#define WF_STATUS_H

#define WF_GOOD 0
#define WF_BAD  1

#define WF_BAD_NOTIMPLEMENTED 2
#define WF_BAD_TIMEOUT        3
#define WF_BAD_BUSY           4
#define WF_BAD_FORMAT         5

#define WF_BAD_NOENTRY 101
#define WF_BAD_NOACCESS 102

typedef int wf_status;

#endif
