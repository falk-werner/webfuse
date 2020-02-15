#ifndef WF_ADAPTER_MOUNTPOINT_H
#define WF_ADAPTER_MOUNTPOINT_H

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_mountpoint;

typedef void
wf_mountpoint_ondispose_fn(
    struct wf_mountpoint * mountpoint);

extern struct wf_mountpoint *
wf_mountpoint_create(
    char const * path);

extern void 
wf_mountpoint_dispose(
    struct wf_mountpoint * mountpoint);

extern char const * 
wf_mountpoint_get_path(
    struct wf_mountpoint const * mountpoint);

extern void
wf_mountpoint_set_ondispose(
    struct wf_mountpoint * mountpoint,
    wf_mountpoint_ondispose_fn * ondispose);

#ifdef __cplusplus
}
#endif


#endif
