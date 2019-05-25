#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>

#include "webfuse_provider.h"

#define SERVICE_TIMEOUT (1 * 1000)

struct args
{
    char const * url;
    bool show_help;
};

static int
parse_args(
    struct args * args,
    int argc,
    char * argv[])
{
    int result = EXIT_FAILURE;
    args->show_help = true;
    args->url = NULL;

    if (2 == argc)
    {        
        result = EXIT_SUCCESS;

        char const * url = argv[1];
        if ((0 != strcmp(url, "-h")) && (0 != strcmp(url, "--help")))
        {
            args->show_help = false;
            args->url = url;
        }
    }
    else
    {
        fprintf(stderr, "error: missing argument\n");
    }
    
    return result;
}

static volatile bool shutdown_requested = false;

static void on_interrupt(int signal_id)
{
	(void) signal_id;
    shutdown_requested = true;
}

static void print_usage()
{
    printf(
        "static-filesystem-provider Copyright (c) 2019, webfuse authors <https://github.com/falk-werner/webfuse>\n"
        "Example of webfuse static filesystem provider\n"
        "\n"
        "Usage: static-filesystem-provider <url>\n"
        "\n"
        "Arguments:\n"
        "\t<url>       URL of webfuse server (required)\n"
        "\t-h, --help  prints this message\n"
        "\n"
        "Example:\n"
        "\tstatic-filesystem-provider ws://localhost:8080/\n"
        "\n"
    );
}

int main(int argc, char* argv[])
{
    signal(SIGINT, &on_interrupt);

    struct args args;
    int result = parse_args(&args, argc, argv);
    if ((EXIT_SUCCESS == result) && (!args.show_help))
    {
        struct wfp_client_config * config = wfp_client_config_create();

        struct wfp_static_filesystem * fs = wfp_static_filesystem_create(config);
        wfp_static_filesystem_add_text(fs, "brummni/hello_world.txt", 0444, "Hello, World!\n");
        wfp_static_filesystem_add_text(fs, "brummni/hello_bob.txt", 0444, "Hello, Bob!\n");
        wfp_static_filesystem_add_text(fs, "brummni/hello_bob.txt", 0444, "Hello, Alice!\n");
        wfp_static_filesystem_add_text(fs, "bla/hello_world.txt", 0444, "Hello, World!\n");
        wfp_static_filesystem_add_text(fs, "foo.txt", 0444, "foo\n");
        wfp_static_filesystem_add_text(fs, "bar.txt", 0444, "bar\n");

        struct wfp_client * client = wfp_client_create(config);
        wfp_client_connect(client, args.url);

        while (!shutdown_requested)
        {
            wfp_client_service(client, SERVICE_TIMEOUT);
        }
        
        wfp_client_dispose(client);
        wfp_static_filesystem_dispose(fs);
        wfp_client_config_dispose(config);
    }

    if (args.show_help)
    {
        print_usage();
    }

    return result;
}
