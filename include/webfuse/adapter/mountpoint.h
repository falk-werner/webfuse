#ifndef WF_ADAPTER_MOUNTPOINT_H
#define WF_ADAPTER_MOUNTPOINT_H

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_mountpoint;

extern struct wf_mountpoint *
wf_mountpoint_create(
    char const * path);

extern void 
wf_mountpoint_release(
    struct wf_mountpoint * mountpoint);

extern char const * 
wf_mountpoint_get_path(
    struct wf_mountpoint const * mountpoint);

#ifdef __cplusplus
}
#endif


#endif
