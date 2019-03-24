#ifndef WSFS_PROVIDER_IMPL_DIRBUFFER_H
#define WSFS_PROVIDER_IMPL_DIRBUFFER_H

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <jansson.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct wsfsp_dirbuffer
{
    json_t * entries;
};

extern struct wsfsp_dirbuffer * wsfsp_impl_dirbuffer_create(void);

extern void wsfsp_impl_dirbuffer_dispose(
    struct wsfsp_dirbuffer * buffer);

extern void wsfsp_impl_dirbuffer_add(
    struct wsfsp_dirbuffer * buffer,
    char const * name,
    ino_t inode);

extern json_t * wsfsp_impl_dirbuffer_take(
    struct wsfsp_dirbuffer * buffer);


#ifdef __cplusplus
}
#endif

#endif
