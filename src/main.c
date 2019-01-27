#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include "wsfs/fuse.h"
#include "wsfs/operations.h"

struct options
{
	char const * document_root;
	char const * server_cert_path;
	char const * server_key_path;
	char const * vhost_name;
	int port;
	int show_help;
};

static struct fuse_opt const option_spec[] =
{
	{"--document_root=%s"   , offsetof(struct options, document_root), 1},
	{"--server_cert_path=%s", offsetof(struct options, server_cert_path), 1},
	{"--server_key_path=%s" , offsetof(struct options, server_key_path), 1},
	{"--vhost_name=%s"      , offsetof(struct options, vhost_name), 1},
	{"--port=%d"            , offsetof(struct options, port), 1},
	{"-h"                   , offsetof(struct options, show_help), 1},
	{"--help"               , offsetof(struct options, show_help), 1},
	FUSE_OPT_END
};

static void show_help(void)
{
	printf(
		"\n"
		"File-system specific options:\n"
		"    --document_root=<s>    Path of www directory (default: not set, www disabled)\n"
		"    --server_cert_path=<s> Path of servers own certificate (default: not set, TLS disabled)\n"
		"    --server_key_path=<s>  Path of servers private key (default: not set, TLS disabled)\n"
		"    --vhost_name=<s>       Name of virtual host (default: \"localhost\")\n"
		"    --port=<d>             Number of servers port (default: 8080)\n"
		"\n");
}

int main(int argc, char * argv[])
{
	struct options options =
	{
		.document_root = NULL,
		.server_cert_path = NULL,
		.server_key_path = NULL,
		.vhost_name = "localhost",
		.port = 8080,
		.show_help = 0
	};

	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
	if (-1 == fuse_opt_parse(&args, &options, option_spec, NULL))
	{
		return EXIT_FAILURE;
	}

	struct fuse_operations operations;
	wsfs_operations_init(&operations);
	
	int result;
	if (!options.show_help)
	{
		result = fuse_main(args.argc, args.argv, &operations, NULL);
	}
	else
	{
		if (0 == fuse_opt_add_arg(&args, "--help"))
		{
			fuse_main(args.argc, args.argv, &operations, NULL);
			show_help();
		}
		else
		{
			fprintf(stderr, "fatal: unable to show help\n");
		}

		result = EXIT_FAILURE;
	}

	fuse_opt_free_args(&args);
	return result;
}

