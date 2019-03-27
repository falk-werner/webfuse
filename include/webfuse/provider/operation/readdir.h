#ifndef WFP_OPERATION_READDIR_H
#define WFP_OPERATION_READDIR_H

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "webfuse/provider/api.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wfp_dirbuffer;
struct wfp_request;

typedef void wfp_readdir_fn(
    struct wfp_request * request,
    ino_t directory,
    void * user_data);

extern WFP_API void wfp_respond_readdir(
    struct wfp_request * request,
    struct wfp_dirbuffer * dirbuffer);

#ifdef __cplusplus
}
#endif

#endif
