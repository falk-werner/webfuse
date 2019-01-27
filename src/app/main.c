#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "wsfs/fuse_wrapper.h"
#include "wsfs/operations.h"
#include "wsfs/server.h"

struct options
{
	struct wsfs_server_config config;
	int show_help;
};

static struct fuse_opt const option_spec[] =
{
	{"--document_root=%s"   , offsetof(struct options, config.document_root), 1},
	{"--server_cert_path=%s", offsetof(struct options, config.cert_path), 1},
	{"--server_key_path=%s" , offsetof(struct options, config.key_path), 1},
	{"--vhost_name=%s"      , offsetof(struct options, config.vhost_name), 1},
	{"--port=%d"            , offsetof(struct options, config.port), 1},
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
		.config = 
		{
			.document_root = NULL,
			.cert_path = NULL,
			.key_path = NULL,
			.vhost_name = strdup("localhost"),
			.port = 8080,
		},
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
		struct wsfs_server * server = wsfs_server_create(&options.config);
		if (NULL != server)
		{
			wsfs_server_start(server);
			struct wsfs_jsonrpc * const rpc = wsfs_server_get_jsonrpc_service(server);

			result = fuse_main(args.argc, args.argv, &operations, rpc);
			wsfs_server_dispose(server);
		}
		else
		{
			fprintf(stderr, "fatal: unable start server\n");
			result = EXIT_FAILURE;
 		}
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
	wsfs_server_config_clear(&options.config);

	return result;
}

