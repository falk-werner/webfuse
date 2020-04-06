/* Why this tool is used:
 *
 *   In order to test webfuse as a fuse filesystem, file system operations should be made.
 *   To check for memory leaks, valgind (memcheck) is used.
 * 
 *   Early tests discovered some ugly behavior of valgrind:
 *     - valgrind intercepts syscalls like open, read and write
 *     - valgrind does not expect that syscalls are handled within the process to be checked
 * 
 *   There is a more or less (un-) documented switch, which changes valgrind's bevahior, but
 *   this caused other problems.
 * 
 *   The second approach used GTests's death tests. Death tests were used quite a while, 
 *   until we discovered a configuration bug when running CTest:
 *     - memory leaks did not lead to test error
 *   
 *   After fixing CTest configuration, memory leaks within the death tests were shown.
 *   Which is correct, since death tests pematurely exits the program an therefore no
 *   cleanup is done.
 * 
 *   Finally, it was decided to use good old popen together with this tool.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include <getopt.h>

struct command;

typedef bool 
command_invoke_fn(
    struct command * command);

struct command
{
    command_invoke_fn * invoke;
    char * file;
    char * arg;
    bool success;
    bool has_arg;
};

static bool print_usage(
    struct command * command)
{
    printf(
        "fs_check, (c) 2020 by Webfuse authors (https://github.com/falk-werner/webfuse)\n"
        "Checks file information\n"
        "\n"
        "Usage:\n"
        "\t./fs_check --command <command> --file <file> [--arg <arg>]\n"
        "\n"
        "Options:\n"
        "\t-c, --command - Check to perform (see below)\n"
        "\t-f, --file    - Path to file to check\n"
        "\t-a, --arg     - Argument, depending on command\n"
        "\n"
        "Commands:\n"
        "\tis_file      - checks, if <file> is a regular file; no arg\n"
        "\tis_dir       - checks, if <file> is a directory; no arg\n"
        "\thas_mode     - checks, if <file> has the mode given in <arg>\n"
        "\thas_size     - checks, if <file> has the size given in <arg>\n"
        "\thas_subdir   - checks, if <file> contains the sub directory given in <arg>\n"
        "\thas_contents - checks, if <file> has the contents given in <arg>\n"
    );

    return command->success;
}

static bool is_file(
    struct command * command)
{
    struct stat buffer;
    int rc = stat(command->file, &buffer);

    return ((0 == rc) && (S_ISREG(buffer.st_mode)));
}

static bool is_dir(
    struct command * command)
{
    struct stat buffer;
    int rc = stat(command->file, &buffer);

    return ((0 == rc) && (S_ISDIR(buffer.st_mode)));
}

static bool has_mode(
    struct command * command)
{
    mode_t mode = (mode_t) atoi(command->arg);
    struct stat buffer;
    int rc = stat(command->file, &buffer);

    return ((0 == rc) && (mode == (buffer.st_mode & 0777)));
}

static bool has_size(
    struct command * command)
{
    int size = atoi(command->arg);
    struct stat buffer;
    int rc = stat(command->file, &buffer);

    return ((0 == rc) && (size == (buffer.st_size)));
}

static bool has_subdir(
    struct command * command)
{
    bool result = false;
    char const * subdir = command->arg;
    DIR * dir = opendir(command->file);
    if (NULL != dir)
    {
        struct dirent * entry = readdir(dir);
        while (NULL != entry)
        {
            if (0 == strcmp(subdir, entry->d_name))
            {
                result = true;
                break;
            }

            entry = readdir(dir);
        }

        closedir(dir);
    }

    return result;
}

static bool has_contents(
    struct command * command)
{
    bool result = false;
    char const * contents = command->arg;
    size_t length = strlen(contents);

    char * buffer = malloc(length);
    FILE * file = fopen(command->file, "rb");
    {
        ssize_t count = fread(buffer, 1, length, file);
        fclose(file);

        result = (count == (ssize_t) length) && (0 == strncmp(buffer, contents, length));
    }

    free(buffer);
    return result;
}

static bool get_command(
    struct command * command,
    char const * name)
{
    static struct { 
        char const * name;
        command_invoke_fn * invoke;
        bool has_arg;
    } commands[] =
    {
        {"is_file"     , &is_file     , false},
        {"is_dir"      , &is_dir      , false},
        {"has_mode"    , &has_mode    , true},
        {"has_size"    , &has_size    , true},
        {"has_subdir"  , &has_subdir  , true},
        {"has_contents", &has_contents, true},
        {NULL, NULL, false}
    };

    for (int i = 0; NULL != commands[i].name; i++)
    {
        if (0 == strcmp(commands[i].name, name))
        {
            command->invoke = commands[i].invoke;
            command->has_arg = commands[i].has_arg;
            return true;
        }
    }

    return false;
}

static void command_init(
    struct command * command,
    int argc,
    char * argv[])
{
    static struct option const options[] =
    {
        {"file"   , required_argument, NULL, 'f'},
        {"command", required_argument, NULL, 'c'},
        {"arg"    , required_argument, NULL, 'a'},
        {"help"   , no_argument      , NULL, 'h'},
        {NULL, 0, NULL, 0}
    };

    command->invoke = &print_usage;
    command->file = NULL;
    command->arg = NULL;
    command->success = true;
    command->has_arg = false;

    optind = 0;
    bool is_finished = false;
    while (!is_finished)
    {
        int option_index = 0;
        int const c = getopt_long(argc, argv, "f:c:a:h", options, &option_index);

        switch(c)
        {
            case -1:
                is_finished = true;
                break;
            case 'c':
                if (!get_command(command, optarg))
                {
                    fprintf(stderr, "error: unknown command\n");
                    command->invoke = &print_usage;
                    command->success = false;
                    is_finished = true;
                }
                break;
            case 'f':
                free(command->file);
                command->file = strdup(optarg);
                break;
            case 'a':
                free(command->arg);
                command->arg = strdup(optarg);
                break;
            case 'h':
                command->invoke = &print_usage;
                break;
            default:
                fprintf(stderr, "error: unknown argument\n");
                command->invoke = &print_usage;
                command->success = false;
                is_finished = true;
                break;
        }
    }

    if (command->success)
    {
        if (NULL == command->file)
        {
            fprintf(stderr, "error: missing required arg: -f\n");
            command->invoke = &print_usage;
            command->success = false;
        }
        else if ((command->has_arg) && (NULL == command->arg))
        {
            fprintf(stderr, "error: missing required arg: -a\n");
            command->invoke = &print_usage;
            command->success = false;
        }
    }
}

static void command_cleanup(
    struct command * command)
{
    free(command->file);
    free(command->arg);
}

int main(int argc, char* argv[])
{
    struct command command;
    command_init(&command, argc, argv);
    
    bool success = command.invoke(&command);

    command_cleanup(&command);
    return success ? EXIT_SUCCESS : EXIT_FAILURE;
}