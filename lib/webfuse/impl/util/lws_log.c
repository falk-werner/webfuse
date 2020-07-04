#include "webfuse/impl/util/lws_log.h"
#include <stdbool.h>
#include <libwebsockets.h>

#define WF_LWSLOG_DISABLE 0

static bool wf_impl_lwslog_is_diabled = false;

void wf_impl_lwslog_disable(void)
{
    if (!wf_impl_lwslog_is_diabled)
    {
	    lws_set_log_level(WF_LWSLOG_DISABLE, NULL);
        wf_impl_lwslog_is_diabled = true;
    }
}


