#ifndef WF_ADAPTER_MOUNTPOINT_H
#define WF_ADAPTER_MOUNTPOINT_H

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_mountpoint;

typedef void
wf_mountpoint_userdata_dispose_fn(
    void * user_data);

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
wf_mountpoint_set_userdata(
    struct wf_mountpoint * mointpoint,
    void * user_data,
    wf_mountpoint_userdata_dispose_fn * dispose);

#ifdef __cplusplus
}
#endif


#endif
