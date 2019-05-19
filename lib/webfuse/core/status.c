#include "webfuse/core/status_intern.h"

#include <errno.h>

 int wf_status_to_rc(wf_status status)
{
	switch(status)
	{
		case WF_GOOD: return 0;
		case WF_BAD_NOTIMPLEMENTED: return -ENOSYS;
		case WF_BAD_TIMEOUT: return -ETIMEDOUT;
		case WF_BAD_BUSY: return -ENOENT;
		case WF_BAD_FORMAT: return -ENOENT;
		case WF_BAD_NOENTRY: return -ENOENT;
		case WF_BAD_ACCESS_DENIED: return -EACCES;
		default: return -ENOENT;
	}
}

char const * wf_status_tostring(wf_status status)
{
	switch(status)
	{
		case WF_GOOD: return "Good";
		case WF_BAD: return "Bad";
		case WF_BAD_NOTIMPLEMENTED: return "Bad (not implemented)";
		case WF_BAD_TIMEOUT: return "Bad (timeout)";
		case WF_BAD_BUSY: return "Bad (busy)";
		case WF_BAD_FORMAT: return "Bad (format)";
		case WF_BAD_NOENTRY: return "Bad (no entry)";
		case WF_BAD_ACCESS_DENIED: return "Bad (access denied)";
		default: return "Bad (unknown)";
	}
}
