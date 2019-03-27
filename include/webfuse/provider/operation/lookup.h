#ifndef WFP_OPERATION_LOOKUP_H
#define WFP_OPERATION_LOOKUP_H

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "webfuse/provider/api.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wfp_request;

typedef void wfp_lookup_fn(
    struct wfp_request * request,
    ino_t parent,
    char const * name,
    void * user_data);

extern WFP_API void wfp_respond_lookup(
    struct wfp_request * request,
    struct stat const * stat);


#ifdef __cplusplus
}
#endif

#endif
