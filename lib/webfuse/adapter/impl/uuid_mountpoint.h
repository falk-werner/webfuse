#ifndef WF_IMPL_UUID_MOUNTPOINT_H
#define WF_IMPL_UUID_MOUNTPOINT_H

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_mountpoint;

extern struct wf_mountpoint *
wf_impl_uuid_mountpoint_create(
    char const * root_path,
    char const * filesystem);

#ifdef __cplusplus
}
#endif

#endif
