#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>

#include <sys/stat.h>
#include <fcntl.h>

#include "wsfs_provider.h"

enum fs_entry_type
{
    FS_FILE,
    FS_DIR
};

struct fs_entry
{
    ino_t parent;
    ino_t inode;
    char const * name;
    int mode;
    enum fs_entry_type type;
    size_t content_length;
    char const * content;
};

struct fs
{
    struct fs_entry const * entries;
};

static struct fs_entry const * fs_getentry(
    struct fs * fs,
    ino_t inode)
{
    for (size_t i = 0; 0 != fs->entries[i].inode; i++)
    {
        struct fs_entry const * entry = &fs->entries[i];
        if (inode == entry->inode)
        {
            return entry;
        }
    } 

    return NULL;
}

static struct fs_entry const * fs_getentry_byname(
    struct fs * fs,
    ino_t parent,
    char const * name)
{
    for( size_t i = 0; 0 != fs->entries[i].inode; i++)
    {
        struct fs_entry const * entry = &fs->entries[i];
        if ((parent == entry->parent) && (0 == strcmp(name, entry->name)))
        {
            return entry;
        }
    }

    return NULL;
}

static void fs_stat(
    struct fs_entry const * entry,
    struct stat * stat)
{
        memset(stat, 0, sizeof(struct stat));

        stat->st_ino = entry->inode;
        stat->st_mode = entry->mode;

        if (FS_DIR == entry->type)
        {
            stat->st_mode |= S_IFDIR;
        }

        if (FS_FILE == entry->type)
        {
            stat->st_mode |= S_IFREG;
            stat->st_size = entry->content_length;
        }
}

static void fs_lookup(
    struct wsfsp_request * request,
    ino_t parent,
    char const * name,
    void * user_data)
{
    struct fs * fs = (struct fs*) user_data;
    struct fs_entry const * entry = fs_getentry_byname(fs, parent, name);
    if (NULL != entry)
    {
        struct stat stat;
        fs_stat(entry, &stat);

        wsfsp_respond_lookup(request, &stat);
    }
    else
    {
        wsfsp_respond_error(request, WSFS_BAD_NOENTRY);
    }
}


static void fs_getattr(
    struct wsfsp_request * request,
    ino_t inode,
    void * user_data)
{
    struct fs * fs = (struct fs*) user_data;
    struct fs_entry const * entry = fs_getentry(fs, inode);

    if (NULL != entry)
    {
        struct stat stat;
        fs_stat(entry, &stat);

        wsfsp_respond_getattr(request, &stat);
    }
    else
    {
        wsfsp_respond_error(request, WSFS_BAD_NOENTRY);
    }
}

static void fs_readdir(
    struct wsfsp_request * request,
    ino_t directory,
    void * user_data)
{
    struct fs * fs = (struct fs*) user_data;

    struct fs_entry const * dir = fs_getentry(fs, directory);
    if ((NULL != dir) && (FS_DIR == dir->type))
    {
        struct wsfsp_dirbuffer * buffer = wsfsp_dirbuffer_create();
        wsfsp_dirbuffer_add(buffer, ".", dir->inode);
        wsfsp_dirbuffer_add(buffer, "..", dir->inode);

        for(size_t i = 0; 0 != fs->entries[i].inode; i++)
        {
            struct fs_entry const * entry = &fs->entries[i];
            if (directory == entry->parent)
            {
                wsfsp_dirbuffer_add(buffer, entry->name, entry->inode);
            }
        }

        wsfsp_respond_readdir(request, buffer);
        wsfsp_dirbuffer_dispose(buffer);
    }
    else
    {
        wsfsp_respond_error(request, WSFS_BAD_NOENTRY);
    }
}

static void fs_open(
    struct wsfsp_request * request,
    ino_t inode,
    int flags,
    void * user_data)
{
    struct fs * fs = (struct fs*) user_data;

    struct fs_entry const * entry = fs_getentry(fs, inode);
    if ((NULL != entry) && (FS_FILE == entry->type))
    {
        if (O_RDONLY == (flags & O_ACCMODE))
        {
            wsfsp_respond_open(request, 0U);
        }
        else
        {
            wsfsp_respond_error(request, WSFS_BAD_NOACCESS);
        }        
    }
    else
    {
        wsfsp_respond_error(request, WSFS_BAD_NOENTRY);
    }
}

static size_t min(size_t const a, size_t const b)
{
    return (a < b) ? a : b;
}

static void fs_read(
    struct wsfsp_request * request,
    ino_t inode,
    uint32_t handle,
    size_t offset,
    size_t length,
    void * user_data)
{
    (void) handle;

    struct fs * fs = (struct fs*) user_data;
    struct fs_entry const * entry = fs_getentry(fs, inode);
    if ((NULL != entry) && (FS_FILE == entry->type))
    {
        if (entry->content_length > offset)
        {
            size_t const remaining = entry->content_length - offset;
            size_t const count = min(remaining, length);

            wsfsp_respond_read(request, &entry->content[offset], count);
        }
        else
        {
            wsfsp_respond_error(request, WSFS_BAD);
        }        
    }
    else
    {
        wsfsp_respond_error(request, WSFS_BAD_NOENTRY);
    }
}

static struct wsfsp_provider fs_provider =
{
    .lookup = &fs_lookup,
    .getattr = &fs_getattr,
    .readdir = &fs_readdir,
    .open = &fs_open,
    .read = &fs_read 
};

static struct wsfsp_client * client;

static void on_interrupt(int signal_id)
{
	(void) signal_id;

	wsfsp_client_shutdown(client);
}

int main(int argc, char* argv[])
{
    (void) argc;
    (void) argv;

    static struct fs_entry const entries[]=
    {
        {.parent = 0, .inode = 1, .name = "<root>", .mode = 0555, .type = FS_DIR},
        {
            .parent = 1,
            .inode = 2,
            .name = "hello.txt",
            .mode = 0555,
            .type = FS_FILE,
            .content="hello, world!",
            .content_length = 13,
        },
        {.parent = 0, .inode = 0, .name = NULL}
    };

    struct fs fs =
    {
        .entries = entries
    };

    signal(SIGINT, &on_interrupt);

    client = wsfsp_client_create(&fs_provider, &fs);
    wsfsp_client_connect(client, "ws://localhost:8080/");

    wsfsp_client_run(client);

    wsfsp_client_dispose(client);
    return EXIT_SUCCESS;
}