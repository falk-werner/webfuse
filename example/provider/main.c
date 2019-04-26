#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>

#include "webfuse_provider.h"

#define SERVICE_TIMEOUT (1 * 1000)

struct config
{
    char * url;
    struct wfp_client_config * client_config;
    bool show_help;
};

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

static void show_help()
{
    printf(
        "webfuse-provider, Copyright (c) 2019, webfuse authors <https://github.com/falk-werner/webfuse>\n"
        "Example for websocket file system provider\n"
        "\n"
        "Usage: webfuse-provider -u <url> [-k <key_path>] [-c <cert_path>]\n"
        "\n"
        "Options:\n"
        "\t-u, --url       URL of webfuse server (required)\n"
        "\t-k, --key_path  Path to private key of provider (default: not set, TLS disabled)\n"
        "\t-c, --cert_path Path to certificate of provider (defautl: not set, TLS disabled)\n"
        "\t-h, --help      prints this message\n"
        "\n"
        "Example:\n"
        "\twebfuse-provider -u ws://localhost:8080/\n"
        "\n"
    );
}

static int parse_arguments(
    int argc,
    char* argv[],
    struct config * config)
{
    static struct option const options[] =
    {
        {"url", required_argument, NULL, 'u'},
        {"key_path", required_argument, NULL, 'k'},
        {"cert_path", required_argument, NULL, 'c'},
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}
    };

    int result = EXIT_SUCCESS;
    bool finished = false;
    while (!finished)
    {
        int option_index = 0;
        int const c = getopt_long(argc, argv, "u:k:c:h", options, &option_index);

        switch (c)
        {
            case -1:
                finished = true;
                break;
            case 'h':
                config->show_help = true;
                finished = true;
                break;
            case 'u':
                free(config->url);
                config->url = strdup(optarg);
                break;
            case 'k':
                wfp_client_config_set_keypath(config->client_config, optarg);
                break;
            case 'c':
                wfp_client_config_set_certpath(config->client_config, optarg);
                break;
            default:
                fprintf(stderr, "error: unknown argument\n");
                finished = true;
                result = EXIT_FAILURE;
                break;
        }

        if (NULL == config->url)
        {
            fprintf(stderr, "error: missing required argument \"-u\"\n");
            result = EXIT_FAILURE;
        }

        if (result != EXIT_SUCCESS)
        {
            config->show_help = true;
        }
    }

    return result;
}

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
    struct wfp_request * request,
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

        wfp_respond_lookup(request, &stat);
    }
    else
    {
        wfp_respond_error(request, WF_BAD_NOENTRY);
    }
}


static void fs_getattr(
    struct wfp_request * request,
    ino_t inode,
    void * user_data)
{
    struct fs * fs = (struct fs*) user_data;
    struct fs_entry const * entry = fs_getentry(fs, inode);

    if (NULL != entry)
    {
        struct stat stat;
        fs_stat(entry, &stat);

        wfp_respond_getattr(request, &stat);
    }
    else
    {
        wfp_respond_error(request, WF_BAD_NOENTRY);
    }
}

static void fs_readdir(
    struct wfp_request * request,
    ino_t directory,
    void * user_data)
{
    struct fs * fs = (struct fs*) user_data;

    struct fs_entry const * dir = fs_getentry(fs, directory);
    if ((NULL != dir) && (FS_DIR == dir->type))
    {
        struct wfp_dirbuffer * buffer = wfp_dirbuffer_create();
        wfp_dirbuffer_add(buffer, ".", dir->inode);
        wfp_dirbuffer_add(buffer, "..", dir->inode);

        for(size_t i = 0; 0 != fs->entries[i].inode; i++)
        {
            struct fs_entry const * entry = &fs->entries[i];
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

static void fs_open(
    struct wfp_request * request,
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
            wfp_respond_open(request, 0U);
        }
        else
        {
            wfp_respond_error(request, WF_BAD_ACCESS_DENIED);
        }        
    }
    else
    {
        wfp_respond_error(request, WF_BAD_NOENTRY);
    }
}

static size_t min(size_t const a, size_t const b)
{
    return (a < b) ? a : b;
}

static void fs_read(
    struct wfp_request * request,
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

            wfp_respond_read(request, &entry->content[offset], count);
        }
        else
        {
            wfp_respond_error(request, WF_BAD);
        }        
    }
    else
    {
        wfp_respond_error(request, WF_BAD_NOENTRY);
    }
}

static volatile bool shutdown_requested = false;

static void on_interrupt(int signal_id)
{
	(void) signal_id;
    shutdown_requested = true;
}

int main(int argc, char* argv[])
{
    struct config config;
    config.url = NULL;
    config.show_help = false;
    config.client_config = wfp_client_config_create();
    int result = parse_arguments(argc, argv, &config);

    if (EXIT_SUCCESS == result)
    {
        static struct fs_entry const entries[]=
        {
            {.parent = 0, .inode = 1, .name = "<root>", .mode = 0555, .type = FS_DIR},
            {
                .parent = 1,
                .inode = 2,
                .name = "hello.txt",
                .mode = 0444,
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

        wfp_client_config_set_userdata(config.client_config, &fs);
        wfp_client_config_set_onlookup(config.client_config, &fs_lookup);
        wfp_client_config_set_ongetattr(config.client_config, &fs_getattr);
        wfp_client_config_set_onreaddir(config.client_config, &fs_readdir);
        wfp_client_config_set_onopen(config.client_config, &fs_open);
        wfp_client_config_set_onread(config.client_config, &fs_read);

        struct wfp_client * client = wfp_client_create(config.client_config);
        wfp_client_connect(client, config.url);

        while (!shutdown_requested)
        {
            wfp_client_service(client, SERVICE_TIMEOUT);
        }

        wfp_client_dispose(client);
    }   

    if (config.show_help)
    {
        show_help();
    }

    free(config.url);
    wfp_client_config_dispose(config.client_config);
    return result;
}