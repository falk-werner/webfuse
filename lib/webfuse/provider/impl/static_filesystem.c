#include "webfuse/provider/impl/static_filesystem.h"
#include "webfuse/provider/client_config.h"
#include "webfuse/provider/dirbuffer.h"
#include "webfuse/provider/operation/error.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define WFP_STATIC_FILESYSTEM_DEFAULT_CAPACITY (16)

struct wfp_static_filesystem_entry
{
    size_t inode;
    size_t parent;
    char * name;
    bool is_file;
    int mode;
    size_t size;
    char * content;
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

static struct wfp_static_filesystem_entry *
wfp_static_filesystem_get_entry(
    struct wfp_static_filesystem * filesystem,
    size_t inode)
{
    struct wfp_static_filesystem_entry * entry = NULL;

    if ((0 < inode) && (inode <= filesystem->size))
    {
        entry = &filesystem->entries[inode - 1];
    }
    
    return entry;
}

static struct wfp_static_filesystem_entry *
wfp_static_filesystem_get_entry_by_name(
    struct wfp_static_filesystem * filesystem,
    size_t parent,
    char const * name)
{
    struct wfp_static_filesystem_entry * entry = NULL;
    for(size_t i = 0; i < filesystem->size; i++)
    {
        struct wfp_static_filesystem_entry * current = &filesystem->entries[i];
        if ((parent == current->parent) && (0 == strcmp(name, current->name)))
        {
            entry = current;
            break;
        }
    }

    return entry;
}

static struct  wfp_static_filesystem_entry *
wfp_static_filesystem_add_entry(
    struct wfp_static_filesystem * filesystem)
{
    struct  wfp_static_filesystem_entry * entry = NULL;

    if (filesystem->size < filesystem->capacity)
    {
        entry = &filesystem->entries[filesystem->size];
        entry->inode = filesystem->size + 1;
        filesystem->size++;
    }
    else
    {
        fprintf(stderr, "fatal: static filessystem: unable to add entry\n");
        exit(EXIT_FAILURE);
    }
    

    return entry;
}

static size_t
wfp_static_filesystem_entry_read(
    size_t offset,
    char * buffer,
    size_t buffer_size,
    void * user_data)
{
    (void) user_data;
    (void) offset;
    (void) buffer;
    (void) buffer_size;

    //struct wfp_static_filesystem_entry * entry = user_data;
    size_t result = 0;

    return result;
}

static void
wfp_static_filesystem_entry_get_info(
    void * user_data,
    int * result_mode,
    size_t * result_size)
{
    struct wfp_static_filesystem_entry * entry = user_data;
    *result_mode = entry->mode;
    *result_size = entry->size;
}


static void
wfp_static_filesystem_add_dir(
    struct wfp_static_filesystem * filesystem,
    size_t parent,
    char const * name
)
{
    struct  wfp_static_filesystem_entry * entry = wfp_static_filesystem_get_entry_by_name(filesystem, parent, name);
    if (NULL == entry)
    {
        entry = wfp_static_filesystem_add_entry(filesystem);
        entry->parent = parent;
        entry->is_file = false;
        entry->mode = 0555;
        entry->name = strdup(name);
        entry->user_data = entry;
        entry->read = &wfp_static_filesystem_entry_read;
        entry->get_info = &wfp_static_filesystem_entry_get_info;
        entry->size = 0;
        entry->content = NULL;
    }    
}

static void wfp_static_filesystem_stat(
    struct wfp_static_filesystem_entry * entry,
    struct stat * stat
)
{
    memset(stat, 0, sizeof(struct stat));

    int mode;
    size_t size;
    entry->get_info(entry->user_data, &mode, &size);   

    stat->st_ino = entry->inode;
    stat->st_size = entry->size;
    stat->st_mode = entry->mode & 0777;
    stat->st_mode |= (entry->is_file) ? S_IFREG: S_IFDIR;
}

static void wfp_static_filesystem_lookup(
    struct wfp_request * request,
    ino_t parent,
    char const * name,
    void * user_data)
{
    struct wfp_static_filesystem * filesystem = user_data;
    struct wfp_static_filesystem_entry * entry = wfp_static_filesystem_get_entry_by_name(filesystem, parent, name);

    if (NULL != entry)
    {
        struct stat stat;
        wfp_static_filesystem_stat(entry, &stat);
        wfp_respond_lookup(request, &stat);
    }
    else
    {
        wfp_respond_error(request, WF_BAD_NOENTRY);
    }
}


static void wfp_static_filesystem_getattr(
    struct wfp_request * request,
    ino_t inode,
    void * user_data)
{
    struct wfp_static_filesystem * filesystem = user_data;
    struct wfp_static_filesystem_entry * entry = wfp_static_filesystem_get_entry(filesystem, inode);

    if (NULL != entry)
    {
        struct stat stat;
        wfp_static_filesystem_stat(entry, &stat);
        wfp_respond_getattr(request, &stat);
    }
    else
    {
        wfp_respond_error(request, WF_BAD_NOENTRY);
    }
}

static void wfp_static_filesystem_readdir(
    struct wfp_request * request,
    ino_t directory,
    void * user_data)
{
    struct wfp_static_filesystem * filesystem = user_data;
    struct wfp_static_filesystem_entry * dir = wfp_static_filesystem_get_entry(filesystem, directory);

    if ((NULL != dir) && (!dir->is_file))
    {
        struct wfp_dirbuffer * buffer = wfp_dirbuffer_create();
        wfp_dirbuffer_add(buffer, ".", dir->inode);
        wfp_dirbuffer_add(buffer, "..", dir->inode);

        for(size_t i = 0; i < filesystem->size; i++)
        {
            struct wfp_static_filesystem_entry const * entry = &filesystem->entries[i];
            if (directory == entry->parent)
            {
                wfp_dirbuffer_add(buffer, entry->name, entry->inode);
            }
        }

        wfp_respond_readdir(request, buffer);
        wfp_dirbuffer_dispose(buffer);
    }
    else
    {
        wfp_respond_error(request, WF_BAD_NOENTRY);
    }
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

        wfp_static_filesystem_add_dir(filesystem, 0, "<root>");

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
    for(size_t i = 0; i < filesystem->size; i++)
    {
        struct wfp_static_filesystem_entry * entry = &filesystem->entries[i];
        free(entry->name);
        free(entry->content);
    }

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
