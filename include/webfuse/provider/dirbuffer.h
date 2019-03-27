#ifndef WF_PROVIDER_DIRBUFFER_H
#define WF_PROVIDER_DIRBUFFER_H

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "webfuse/provider/api.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wfp_dirbuffer;

extern WFP_API struct wfp_dirbuffer * wfp_dirbuffer_create(void);

extern WFP_API void wfp_dirbuffer_dispose(
    struct wfp_dirbuffer * buffer);

extern WFP_API void wfp_dirbuffer_add(
    struct wfp_dirbuffer * buffer,
    char const * name,
    ino_t inode);

#ifdef __cplusplus
}
#endif


#endif
