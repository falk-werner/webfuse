#include "wsfs/status.h"

#include <errno.h>

 int wsfs_status_to_rc(wsfs_status status)
{
	switch(status)
	{
		case WSFS_GOOD: return 0;
		case WSFS_BAD_NOTIMPLEMENTED: return -ENOSYS;
		case WSFS_BAD_TIMEOUT: return -ETIMEDOUT;
		case WSFS_BAD_BUSY: return -ENOENT;
		case WSFS_BAD_FORMAT: return -ENOENT;
		case WSFS_BAD_NOENTRY: return -ENOENT;
		case WSFS_BAD_NOACCESS: return -EACCES;
		default: return -ENOENT;
	}
}

char const * wsfs_status_tostring(wsfs_status status)
{
	switch(status)
	{
		case WSFS_GOOD: return "Good";
		case WSFS_BAD: return "Bad";
		case WSFS_BAD_NOTIMPLEMENTED: return "Bad (not implelemted)";
		case WSFS_BAD_TIMEOUT: return "Bad (timeout)";
		case WSFS_BAD_BUSY: return "Bad (busy)";
		case WSFS_BAD_FORMAT: return "Bad (format)";
		case WSFS_BAD_NOENTRY: return "Bad (no entry)";
		case WSFS_BAD_NOACCESS: return "Bad (no access)";
		default: return "Bad (unknown)";
	}
}
