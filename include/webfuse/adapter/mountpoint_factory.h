#ifndef WF_ADAPTER_MOUNTPOINT_FACTORY_H
#define WF_ADAPTER_MOUNTPOINT_FACTORY_H

#include <webfuse/adapter/api.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_mountpoint;

typedef struct wf_mountpoint *
wf_create_mountpoint_fn(
    char const * filesystem,
    void * user_data);


#ifdef __cplusplus
}
#endif


#endif
