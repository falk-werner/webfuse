#ifndef WFP_IMPL_STATIC_FILESYSTEM_H
#define WFP_IMPL_STATIC_FILESYSTEM_H

#include "webfuse/provider/static_filesystem.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern struct wfp_static_filesystem *
wfp_impl_static_filesystem_create(
    struct wfp_client_config * config);

extern void
wfp_impl_static_filesystem_dispose(
    struct wfp_static_filesystem * filesystem);

extern void
wfp_impl_static_filesystem_add(
    struct wfp_static_filesystem * filesystem,
    char const * path,
    int mode,
    char const * content,
    size_t length);

extern void
wfp_impl_static_filesystem_add_text(
    struct wfp_static_filesystem * filesystem,
    char const * path,
    int mode,
    char const * content);

extern void
wfp_impl_static_filesystem_add_file(
    struct wfp_static_filesystem * filesystem,
    char const * path,
    char const * filename);

extern void
wfp_impl_static_filesystem_add_generic(
    struct wfp_static_filesystem * filesystem,
    char const * path,
    wfp_static_filesystem_read_fn * read,
    wfp_static_filesystem_get_info_fn * get_info,
    void * user_data);

#ifdef __cplusplus
}
#endif

#endif
