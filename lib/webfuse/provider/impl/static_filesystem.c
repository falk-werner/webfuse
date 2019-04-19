#include "webfuse/provider/impl/static_filesystem.h"
#include "webfuse/provider/client_config.h"
#include "webfuse/provider/operation/error.h"

#include <stdlib.h>
#include <stdbool.h>

#define WFP_STATIC_FILESYSTEM_DEFAULT_CAPACITY (16)

struct wfp_static_filesystem_entry
{
    struct wfp_static_filesystem * parent;
    char const * name;
    bool is_file;
    int mode;
    size_t size;
    wfp_static_filesystem_read_fn * read;
    wfp_static_filesystem_get_info_fn * get_info;
    void * user_data;
};

struct wfp_static_filesystem
{
    struct wfp_static_filesystem_entry * entries;
    size_t size;
    size_t capacity;
};

static void wfp_static_filesystem_lookup(
    struct wfp_request * request,
    ino_t parent,
    char const * name,
    void * user_data)
{
    (void) parent;
    (void) name;
    (void) user_data;

    wfp_respond_error(request, WF_BAD_NOENTRY);
}


static void wfp_static_filesystem_getattr(
    struct wfp_request * request,
    ino_t inode,
    void * user_data)
{
    (void) inode;
    (void) user_data;

    wfp_respond_error(request, WF_BAD_NOENTRY);
}

static void wfp_static_filesystem_readdir(
    struct wfp_request * request,
    ino_t directory,
    void * user_data)
{
    (void) directory;
    (void) user_data;

    wfp_respond_error(request, WF_BAD_NOENTRY);
}

static void wfp_static_filesystem_open(
    struct wfp_request * request,
    ino_t inode,
    int flags,
    void * user_data)
{
    (void) inode;
    (void) flags;
    (void) user_data;

    wfp_respond_error(request, WF_BAD_NOENTRY);
}

static void wfp_static_filesystem_read(
    struct wfp_request * request,
    ino_t inode,
    uint32_t handle,
    size_t offset,
    size_t length,
    void * user_data)
{
    (void) inode;
    (void) handle;
    (void) offset;
    (void) length;
    (void) user_data;

    wfp_respond_error(request, WF_BAD_NOENTRY);
}


struct wfp_static_filesystem *
wfp_impl_static_filesystem_create(
    struct wfp_client_config * config)
{
    (void) config;

    struct wfp_static_filesystem * filesystem = malloc(sizeof(struct wfp_static_filesystem));
    if (NULL != filesystem)
    {
        filesystem->entries = malloc(sizeof(struct wfp_static_filesystem_entry) * WFP_STATIC_FILESYSTEM_DEFAULT_CAPACITY);
        filesystem->size = 0;
        filesystem->capacity = WFP_STATIC_FILESYSTEM_DEFAULT_CAPACITY;

        wfp_client_config_set_userdata(config, filesystem);
        wfp_client_config_set_onlookup(config, &wfp_static_filesystem_lookup);
        wfp_client_config_set_ongetattr(config, &wfp_static_filesystem_getattr);
        wfp_client_config_set_onreaddir(config, &wfp_static_filesystem_readdir);
        wfp_client_config_set_onopen(config, &wfp_static_filesystem_open);
        wfp_client_config_set_onread(config, &wfp_static_filesystem_read);


    }

    return filesystem;
}

void
wfp_impl_static_filesystem_dispose(
    struct wfp_static_filesystem * filesystem)
{
    free(filesystem->entries);
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
