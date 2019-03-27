#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <unistd.h>
#include <getopt.h>

#include <openssl/conf.h>
#include <openssl/opensslv.h>
#include <openssl/engine.h>
#include <openssl/evp.h>
#include <jansson.h>
#include <userdb.h>


struct args
{
    char * file;
    char * command;
    char * username;
    char * password;
    char * pepper;
    bool show_help;
};

typedef int command_invoke_fn(
    struct args * args);

struct command
{
    char const * name;
    command_invoke_fn * invoke;
};

static void print_usage(void)
{
    printf(
        "webfuse-passwd, Copyright (c) 2019, webfuse authors <https://github.com/falk-werner/webfuse>\n"
        "Manage webfuse passwd file\n"
        "\n"
        "Usage: webfuse-passwd -f <file> -c <command> [-u <username>] [-p <password>] [-P <pepper>]\n"
        "\n"
        "Options:\n"
        "\t-f, --file     Path of wf passwd file\n"
        "\t-c, --command  Command to execute\n"
        "\t-u, --username Name of user\n"
        "\t-p, --password Password of user\n"
        "\t-P, --pepper   pepper\n"
        "\t-h, --help     Shows this message\n"
        "\n"
        "Commands:\n"
        "\tcreate Creates an empty passwd file (or cleans an existing)\n"
        "\t       Example: webfuse-passwd -f passwd.json -c create\n"
        "\tadd    Adds or replaces a user\n"
        "\t       Example: webfuse-passwd -f passwd.json -c add -u bob -p secret\n"
        "\tremove Removes a user\n"
        "\t       Example: webfuse-passwd -f passwd.json -c remove -u bob\n"
        "\tcheck  Checks password of a user\n"
        "\t       Example: webfuse-passwd -f passwd.json -c check -u bob -p secret\n"
        "\n"
    );
}

static int parse_args(struct args * args, int argc, char * argv[])
{
	static struct option const options[] =
	{
		{"file", required_argument, NULL, 'f'},
		{"command", required_argument, NULL, 'c'},
		{"username", required_argument, NULL, 'u'},
		{"password", required_argument, NULL, 'p'},
		{"Pepper", required_argument, NULL, 'P'},
		{"help", required_argument, NULL, 'h'},
		{NULL, 0, NULL, 0}
	};

	int result = EXIT_SUCCESS;
	bool finished = false;
	while ((!finished) && (EXIT_SUCCESS == result))
	{
		int option_index = 0;
		int const c = getopt_long(argc, argv, "f:c:u:p:P:h", options, &option_index);

		switch (c)
		{
			case -1:
				finished = true;
				break;
			case 'h':
				args->show_help = true;
				finished = true;
				break;
			case 'f':
                free(args->file);
                args->file = strdup(optarg);
				break;
			case 'c':
                free(args->command);
                args->command = strdup(optarg);
				break;
			case 'u':
                free(args->username);
                args->username = strdup(optarg);
				break;
			case 'p':
                free(args->password);
                args->password = strdup(optarg);
				break;
			case 'P':
                free(args->pepper);
                args->pepper = strdup(optarg);
				break;
			default:
				fprintf(stderr, "error: unknown argument\n");
				result = EXIT_FAILURE;
				break;
		}
	}

    if ((result == EXIT_SUCCESS) && (!args->show_help))
    {
        if (NULL == args->file)
        {
            fprintf(stderr, "error: missing file\n");
            args->show_help = true;
            result = EXIT_FAILURE;
        }
        else if (NULL == args->command)
        {
            fprintf(stderr, "error: missing command\n");
            args->show_help = true;
            result = EXIT_FAILURE;
        }        
    }

    return result;
}

static void args_init(struct args * args)
{
    args->file = NULL;
    args->command = NULL;
    args->username = NULL;
    args->password = NULL;
    args->pepper = strdup("");
    args->show_help = false;
}

static int create_passwd(struct args * args)
{
    struct userdb * db = userdb_create(args->pepper);
    bool result = userdb_save(db, args->file);
    userdb_dispose(db);

    return (result) ? EXIT_SUCCESS : EXIT_FAILURE;
}

static int add_user(struct args * args)
{
    if (NULL == args->username)
    {
        fprintf(stderr, "error: missing username");
        args->show_help = true;
        return EXIT_FAILURE;
    }

    if (NULL == args->password)
    {
        fprintf(stderr, "error: missing password");
        args->show_help = true;
        return EXIT_FAILURE;
    }

    struct userdb * db = userdb_create(args->pepper);
    userdb_load(db, args->file);
    userdb_add(db, args->username, args->password);
    bool result = userdb_save(db, args->file);
    userdb_dispose(db);

    return (result) ? EXIT_SUCCESS : EXIT_FAILURE;
}

static int remove_user(struct args * args)
{
    if (NULL == args->username)
    {
        fprintf(stderr, "error: missing username");
        args->show_help = true;
        return EXIT_FAILURE;
    }

    struct userdb * db = userdb_create(args->pepper);
    userdb_load(db, args->file);
    userdb_remove(db, args->username);
    bool result = userdb_save(db, args->file);
    userdb_dispose(db);

    return (result) ? EXIT_SUCCESS : EXIT_FAILURE;
}

static int check_password(struct args * args)
{
    if (NULL == args->username)
    {
        fprintf(stderr, "error: missing username");
        args->show_help = true;
        return EXIT_FAILURE;
    }

    if (NULL == args->password)
    {
        fprintf(stderr, "error: missing password");
        args->show_help = true;
        return EXIT_FAILURE;
    }

    struct userdb * db = userdb_create(args->pepper);
    userdb_load(db, args->file);
    bool result = userdb_check(db, args->username, args->password);
    userdb_dispose(db);

    printf("%s\n", (result) ? "OK" : "FAILURE");
    return (result) ? EXIT_SUCCESS : EXIT_FAILURE;
}

static int invoke_invalid_command(struct args * args)
{
    (void) args;

    fprintf(stderr, "error: unknown command\n");
    return EXIT_FAILURE;
}

static struct command const commands[] =
{
    {"create", &create_passwd},
    {"add", &add_user},
    {"remove", &remove_user},
    {"check", &check_password},
    {NULL, NULL}
};

static struct command const invalid_command =
{
    "<invalid>",
    &invoke_invalid_command
};

static struct command const * get_command(char const * name)
{
    for(size_t i = 0; NULL != commands[i].name; i++)
    {
        if (0 == strcmp(name, commands[i].name))
        {
            return &commands[i];
        }
    }

    return &invalid_command;
}

static void args_cleanup(struct args * args)
{
    free(args->file);
    free(args->command);
    free(args->username);
    free(args->password);
    free(args->pepper);
}

static void openssl_cleanup(void)
{
    FIPS_mode_set(0);
    ENGINE_cleanup();
    CONF_modules_unload(1);
    EVP_cleanup();
    CRYPTO_cleanup_all_ex_data();
    ERR_free_strings();
}

int main(int argc, char * argv[])
{
    OPENSSL_init();
    OPENSSL_add_all_algorithms_conf();

    struct args args;
    args_init(&args);
    int result = parse_args(&args, argc, argv);
    if ((EXIT_SUCCESS == result) && (!args.show_help))
    {
        struct command const * command = get_command(args.command);
        result = command->invoke(&args);
    }

    if (args.show_help)
    {
        print_usage();
    }

    args_cleanup(&args);
    openssl_cleanup();
    return result;
}