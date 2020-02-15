#ifndef WF_IMPL_MOUNTPOINT_H
#define WF_IMPL_MOUNTPOINT_H

#ifdef __cplusplus
extern "C"
{
#endif

extern struct wf_mountpoint *
wf_impl_mountpoint_create(
    char const * path);

extern void 
wf_impl_mountpoint_dispose(
    struct wf_mountpoint * mountpoint);

extern char const * 
wf_impl_mountpoint_get_path(
    struct wf_mountpoint const * mountpoint);


#ifdef __cplusplus
}
#endif

#endif
