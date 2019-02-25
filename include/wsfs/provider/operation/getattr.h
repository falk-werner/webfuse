#ifndef WSFSP_OPERATION_GETATTR_H
#define WSFSP_OPERATION_GETATTR_H

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "wsfs/provider/api.h"

struct wsfsp_request;

typedef void wsfsp_getattr_fn(
    struct wsfsp_request * request,
    ino_t inode,
    void * user_data);

#ifdef __cplusplus
extern "C"
{
#endif


extern WSFSP_API void wsfsp_respond_getattr(
    struct wsfsp_request * request,
    struct stat const * stat);


#ifdef __cplusplus
}
#endif

#endif
