#include "webfuse/core/lws_log.h"
#include <stdbool.h>
#include <libwebsockets.h>

#define WF_LWSLOG_DISABLE 0

static bool wf_lwslog_is_diabled = false;

void wf_lwslog_disable(void)
{
    if (!wf_lwslog_is_diabled)
    {
	    lws_set_log_level(WF_LWSLOG_DISABLE, NULL);
        wf_lwslog_is_diabled = true;
    }
}


