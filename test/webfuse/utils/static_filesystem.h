#ifndef WFP_STATIC_FILESYSTEM_H
#define WFP_STATIC_FILESYSTEM_H

#ifndef __cplusplus
#include <stddef.h>
#else
#include <cstddef>
using ::std::size_t;
#endif

#include <webfuse/provider/api.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct wfp_client_config;

struct wfp_static_filesystem;

typedef size_t
wfp_static_filesystem_read_fn(
    size_t offset,
    char * buffer,
    size_t buffer_size,
    void * user_data);

typedef void
wfp_static_filesystem_get_info_fn(
    void * user_data,
    int * result_mode,
    size_t * result_size);


extern WFP_API struct wfp_static_filesystem *
wfp_static_filesystem_create(
    struct wfp_client_config * config);

extern WFP_API void
wfp_static_filesystem_dispose(
    struct wfp_static_filesystem * filesystem);

extern WFP_API void
wfp_static_filesystem_add(
    struct wfp_static_filesystem * filesystem,
    char const * path,
    int mode,
    char const * content,
    size_t length);

extern WFP_API void
wfp_static_filesystem_add_text(
    struct wfp_static_filesystem * filesystem,
    char const * path,
    int mode,
    char const * content);

extern void
wfp_static_filesystem_set_user_data(
    struct wfp_static_filesystem * filesystem,
    void * user_data);

extern void *
wfp_static_filesystem_get_user_data(
    struct wfp_static_filesystem * filesystem);

#ifdef __cplusplus
}
#endif

#endif
