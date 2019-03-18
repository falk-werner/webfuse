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

#define HASH_ALGORITHM "sha512"
#define PASSWD_FORMAT_MAJOR 1
#define PASSWD_FORMAT_MINOR 0

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
        "wsfs-passwd, Copyright (c) 2019 authors <https://github.com/falk-werner/fuse-wsfs>\n"
        "Manage wsfs passwd file\n"
        "\n"
        "Usage: wsfs-passwd -f <file> -c <command> [-u <username>] [-p <password>] [-P <pepper>]\n"
        "\n"
        "Options:\n"
        "\t-f, --file     Path of wsfs passwd file\n"
        "\t-c, --command  Command to execute\n"
        "\t-u, --username Name of user\n"
        "\t-p, --password Password of user\n"
        "\t-P, --pepper   pepper\n"
        "\t-h, --help     Shows this message\n"
        "\n"
        "Commands:\n"
        "\tcreate Creates an empty passwd file (or cleans an existing)\n"
        "\t       Example: wsfs-passwd -f passwd.json -c create\n"
        "\tadd    Adds or replaces a user\n"
        "\t       Example: wsfs-passwd -f passwd.json -c add -u bob -p secret\n"
        "\tremove Removes a user\n"
        "\t       Example: wsfs-passwd -f passwd.json -c remove -u bob\n"
        "\tcheck  Checks password of a user\n"
        "\t       Example: wsfs-passwd -f passwd.json -c check -u bob -p secret\n"
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
    json_t * db = json_object();

    json_t * meta = json_object();
    json_object_set_new(meta, "type", json_string("wsfs-passwd"));
    json_object_set_new(meta, "major", json_integer(PASSWD_FORMAT_MAJOR));
    json_object_set_new(meta, "minor", json_integer(PASSWD_FORMAT_MINOR));
    json_object_set_new(meta, "hash_alorithm", json_string(HASH_ALGORITHM));
    json_object_set_new(db, "meta", meta);

    json_t * users = json_object();
    json_object_set_new(db, "users", users);

    int result = json_dump_file(db, args->file, JSON_INDENT(2));
    json_decref(db);

    return (0 == result) ? EXIT_SUCCESS : EXIT_FAILURE;
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

    

    puts("add");
    return EXIT_FAILURE;
}

static int remove_user(struct args * args)
{
    puts("remove");
    return EXIT_FAILURE;
}

static int check_password(struct args * args)
{
    puts("check");
    return EXIT_FAILURE;
}

static int invoke_invalid_command(struct args * args)
{
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

static char hex_char(unsigned char value)
{
    switch (value)
    {
        case  0x00: return '0';
        case  0x01: return '1';
        case  0x02: return '2';
        case  0x03: return '3';
        case  0x04: return '4';
        case  0x05: return '5';
        case  0x06: return '6';
        case  0x07: return '7';
        case  0x08: return '8';
        case  0x09: return '9';
        case  0x0a: return 'a';
        case  0x0b: return 'b';
        case  0x0c: return 'c';
        case  0x0d: return 'd';
        case  0x0e: return 'e';
        case  0x0f: return 'f';
        default: return '?';
    }
}

static char * to_hex(unsigned char const * value, size_t length)
{
    char * result = malloc((2 * length) + 1);
    if (NULL != result)
    {
        for (size_t i = 0, j = 0; i < length; i++, j+=2)
        {
            unsigned char high = (value[i] >> 4) & 0x0f;
            unsigned char low =   value[i]       & 0x0f;

            result[j    ] = hex_char(high);
            result[j + 1] = hex_char(low);
        }

        result[2 * length] = '\0';
    }

    return result;
}

static char * get_password_hash(
    char const * password,
    char const * salt,
    char * pepper)
{
    EVP_MD const * digest = EVP_get_digestbyname(HASH_ALGORITHM);
    if (NULL == digest)
    {
        fprintf(stderr, "error: hash algorithm %s not supported\n", HASH_ALGORITHM);
        exit(EXIT_FAILURE);
    }

    char * result = NULL;
    unsigned int hash_size = digest->md_size;
    unsigned char * hash = malloc(hash_size);

    if (NULL != hash)
    {
        EVP_MD_CTX context;        
        EVP_MD_CTX_init(&context);
        EVP_DigestInit_ex(&context, digest, NULL);        
        EVP_DigestUpdate(&context, password, strlen(password));
        EVP_DigestUpdate(&context, salt, strlen(salt));
        EVP_DigestUpdate(&context, pepper, strlen(pepper));
        EVP_DigestFinal_ex(&context, hash, &hash_size);
        EVP_MD_CTX_cleanup(&context);

        result = to_hex(hash, hash_size);
        free(hash);
    }

    return result;
}

static void openssl_cleanup(void)
{
    FIPS_mode_set(0);
    ENGINE_cleanup();
    CONF_modules_unload(1);
    EVP_cleanup();
    CRYPTO_cleanup_all_ex_data();
    ERR_remove_state(0);
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