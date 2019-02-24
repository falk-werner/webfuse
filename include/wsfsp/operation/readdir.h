#ifndef WSFSP_OPERATION_READDIR_H
#define WSFSP_OPERATION_READDIR_H

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "wsfsp/api.h"

struct wsfsp_dirbuffer;
struct wsfsp_request;

typedef void wsfsp_readdir_fn(
    struct wsfsp_request * request,
    ino_t directory,
    void * user_data);


#ifdef __cplusplus
extern "C"
{
#endif

extern WSFSP_API void wsfsp_respond_readdir(
    struct wsfsp_request * request,
    struct wsfsp_dirbuffer * dirbuffer);

#ifdef __cplusplus
}
#endif

#endif
