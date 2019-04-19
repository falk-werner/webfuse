#include "webfuse/provider/impl/static_filesystem.h"
#include <stdlib.h>

struct wfp_static_filesystem
{
    int dummy;
};

struct wfp_static_filesystem *
wfp_impl_static_filesystem_create(
    struct wfp_client_config * config)
{
    (void) config;

    struct wfp_static_filesystem * filesystem = malloc(sizeof(struct wfp_static_filesystem));
    if (NULL != filesystem)
    {
        // ToDo: implement me
    }

    return filesystem;
}

void
wfp_impl_static_filesystem_dispose(
    struct wfp_static_filesystem * filesystem)
{
    free(filesystem);
}

void
wfp_impl_static_filesystem_add(
    struct wfp_static_filesystem * filesystem,
    char const * path,
    int mode,
    char const * content,
    size_t length)
{
    (void) filesystem;
    (void) path;
    (void) mode;
    (void) content;
    (void) length;

    // ToDo: implement me
}

void
wfp_impl_static_filesystem_add_text(
    struct wfp_static_filesystem * filesystem,
    char const * path,
    int mode,
    char const * content)
{
    (void) filesystem;
    (void) path;
    (void) mode;
    (void) content;

    // ToDo: implement me
}

void
wfp_impl_static_filesystem_add_file(
    struct wfp_static_filesystem * filesystem,
    char const * path,
    char const * filename)
{
    (void) filesystem;
    (void) path;
    (void) filename;

    // ToDo: implement me
}

void
wfp_impl_static_filesystem_add_generic(
    struct wfp_static_filesystem * filesystem,
    char const * path,
    wfp_static_filesystem_read_fn * read,
    wfp_static_filesystem_get_info_fn * get_info,
    void * user_data)
{
    (void) filesystem;
    (void) path;
    (void) read;
    (void) get_info;
    (void) user_data;

    // ToDo: implement me
}
