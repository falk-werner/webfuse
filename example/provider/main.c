#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>

#include "wsfs_provider.h"


struct fs_dir 
{
    ino_t parent;
    ino_t inode;
    char const * name;
};

struct fs_file
{
    ino_t parent;
    ino_t inode;
    char const * name;
    char const * content;
    size_t content_length;
    bool is_executable;
};

struct fs
{
    struct fs_dir const * directories;
    struct fs_file const * files;
};

static void fs_lookup(
    struct wsfsp_request * request,
    ino_t parent,
    char const * name,
    void * user_data)
{
    (void) parent;
    (void) name;
    (void) user_data;

    puts("lookup");
    wsfsp_respond_error(request, -1);
}

static void fs_getattr(
    struct wsfsp_request * request,
    ino_t inode,
    void * user_data)
{
    (void) inode;
    (void) user_data;

    puts("getattr");
    wsfsp_respond_error(request, -1);
}

static void fs_readdir(
    struct wsfsp_request * request,
    ino_t directory,
    void * user_data)
{
    (void) directory;
    (void) user_data;

    puts("readdir");
    wsfsp_respond_error(request, -1);
}

static void fs_open(
    struct wsfsp_request * request,
    ino_t inode,
    int flags,
    void * user_data)
{
    (void) inode;
    (void) flags;
    (void) user_data;

    puts("open");
    wsfsp_respond_error(request, -1);
}

static void fs_read(
    struct wsfsp_request * request,
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

    wsfsp_respond_error(request, -1);
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

    static struct fs_dir const directories[]=
    {
        {.parent = 0, .inode = 1, .name = "<root>"},
        {.parent = 0, .inode = 0, .name = NULL}
    };

    static struct fs_file const files[] =
    {
        {
            .parent = 1,
            .inode = 2,
            .name = "hello.txt",
            .content="hello, world!",
            .content_length = 13,
            .is_executable = false
        },
        {.parent = 0, .inode = 0}
    };

    struct fs fs =
    {
        .directories = directories,
        .files = files
    };

    signal(SIGINT, &on_interrupt);

    client = wsfsp_client_create(&fs_provider, &fs);
    wsfsp_client_connect(client, "ws://localhost:8080/");

    wsfsp_client_run(client);

    wsfsp_client_dispose(client);
    return EXIT_SUCCESS;
}