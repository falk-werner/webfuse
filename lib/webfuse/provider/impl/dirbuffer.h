#ifndef WF_PROVIDER_IMPL_DIRBUFFER_H
#define WF_PROVIDER_IMPL_DIRBUFFER_H

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <jansson.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct wfp_dirbuffer
{
    json_t * entries;
};

extern struct wfp_dirbuffer * wfp_impl_dirbuffer_create(void);

extern void wfp_impl_dirbuffer_dispose(
    struct wfp_dirbuffer * buffer);

extern void wfp_impl_dirbuffer_add(
    struct wfp_dirbuffer * buffer,
    char const * name,
    ino_t inode);

extern json_t * wfp_impl_dirbuffer_take(
    struct wfp_dirbuffer * buffer);


#ifdef __cplusplus
}
#endif

#endif
