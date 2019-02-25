#ifndef WSFS_PROVIDER_DIRBUFFER_H
#define WSFS_PROVIDER_DIRBUFFER_H

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "wsfs/provider/api.h"

struct wsfsp_dirbuffer;

#ifdef __cplusplus
extern "C"
{
#endif

extern WSFSP_API struct wsfsp_dirbuffer * wsfsp_dirbuffer_init(void);

extern WSFSP_API void wsfsp_dirbuffer_dispose(
    struct wsfsp_dirbuffer * buffer);

extern WSFSP_API void wsfsp_dirbuffer_add(
    struct wsfsp_dirbuffer * buffer,
    char const * name,
    ino_t inode);

#ifdef __cplusplus
}
#endif


#endif
