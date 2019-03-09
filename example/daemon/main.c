#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include <signal.h>

#include <unistd.h>
#include <getopt.h>

#include <wsfs_adapter.h>

struct args
{
	struct wsfs_server_config * config;
	bool show_help;
};

static struct wsfs_server * server;

static void show_help(void)
{
	printf(
		"wsfsd, Copyright (c) 2019, Falk Werner\n"
		"Websocket file system daemon\n"
		"\n"
		"Usage: wsfsd [m <mount_point>] [-d <document_root] [-n <vhost_name>] [-p <port>]\n"
		"            [-c <server_cert_path> -k] [<server_key_path>]\n"
		"\n"
		"Options:\n"
		"\t-m, --mount_point      Path of mount point (required)\n"
		"\t-d, --document_root    Path of www directory (default: not set, www disabled)\n"
		"\t-c, --server_cert_path Path of servers own certificate (default: not set, TLS disabled)\n"
		"\t-k, --server_key_path  Path of servers private key (default: not set, TLS disabled)\n"
		"\t-n, --vhost_name       Name of virtual host (default: \"localhost\")\n"
		"\t-p, --port             Number of servers port (default: 8080)\n"
		"\n");
}

static int parse_arguments(int argc, char * argv[], struct args * args)
{
	static struct option const options[] =
	{
		{"mount_point", required_argument, NULL, 'm'},
		{"document_root", required_argument, NULL, 'd'},
		{"server_cert_path", required_argument, NULL, 'c'},
		{"server_key_path", required_argument, NULL, 'k'},
		{"vhost_name", required_argument, NULL, 'n'},
		{"port", required_argument, NULL, 'p'},
		{"help", no_argument, NULL, 'h'},
		{NULL, 0, NULL, 0}
	};

	bool result = EXIT_SUCCESS;
	bool finished = false;
	bool has_mountpoint = false;
	while ((!finished) && (EXIT_SUCCESS == result))
	{
		int option_index = 0;
		int const c = getopt_long(argc, argv, "m:d:c:k:n:p:h", options, &option_index);

		switch (c)
		{
			case -1:
				finished = true;
				break;
			case 'h':
				args->show_help = true;
				finished = true;
				break;
			case 'm':
				wsfs_server_config_set_mountpoint(args->config, optarg);
				has_mountpoint = true;
				break;
			case 'd':
				wsfs_server_config_set_documentroot(args->config, optarg);
				break;
			case 'c':
				wsfs_server_config_set_certpath(args->config, optarg);
				break;
			case 'k':
				wsfs_server_config_set_keypath(args->config, optarg);
				break;
			case 'n':
				wsfs_server_config_set_vhostname(args->config, optarg);
				break;
			case 'p':
				wsfs_server_config_set_port(args->config, atoi(optarg));
				break;
			default:
				fprintf(stderr, "error: unknown argument\n");
				result = EXIT_FAILURE;
				break;
		}
	}

	if ((EXIT_SUCCESS == result) && (!args->show_help))
	{
		if (!has_mountpoint)
		{
			fprintf(stderr, "error: missing mount point\n");
			result = EXIT_FAILURE;
		}
	}

	if (EXIT_SUCCESS != result)
	{
		args->show_help = true;
	}

	return result;
}

static void on_interrupt(int signal_id)
{
	(void) signal_id;

	wsfs_server_shutdown(server);
}

int main(int argc, char * argv[])
{
	struct args args;
	args.config = wsfs_server_config_create();
	wsfs_server_config_set_vhostname(args.config, "localhost");
	wsfs_server_config_set_port(args.config, 8080);
	args.show_help = false;

	int result = parse_arguments(argc, argv, &args);
	
	if (!args.show_help)
	{
		signal(SIGINT, on_interrupt);
		server = wsfs_server_create(args.config);
		if (NULL != server)
		{
			wsfs_server_run(server);
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
		show_help();
	}

	wsfs_server_config_dispose(args.config);
	return result;
}

