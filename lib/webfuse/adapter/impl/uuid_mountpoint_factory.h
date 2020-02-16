#ifndef WF_IMPL_UUID_MOUNTPOINT_FACTORY_H
#define WF_IMPL_UUID_MOUNTPOINT_FACTORY_H

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_mountpoint;

extern void *
wf_impl_uuid_mountpoint_factory_create(
    char * root_path);

extern void
wf_impl_uuid_mountpoint_factory_dispose(
    void * user_data);

extern struct wf_mountpoint *
wf_impl_uuid_mountpoint_factory_create_mountpiont(
    char const * filesystem,
    void * user_data);


#ifdef __cplusplus
}
#endif

#endif
