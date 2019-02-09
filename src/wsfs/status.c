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
		case WSFS_BAD_NOENTRY: return -ENOENT;
		case WSFS_BAD_NOACCESS: return -EACCES;
		default: return -ENOENT;
	}
}
