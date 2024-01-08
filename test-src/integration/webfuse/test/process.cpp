#include "webfuse/test/process.hpp"

#include <sys/wait.h>
#include <fcntl.h>

#include <csignal>
#include <cstring>
#include <cstdlib>

#include <stdexcept>

#ifndef closefrom
namespace
{

void closefrom(int fd)
{
    int const max_fd = sysconf(_SC_OPEN_MAX);
    for(int i = fd; i < max_fd; i++)
    {
        close(i);
    }
}

}
#endif


namespace webfuse
{

process::process(std::vector<std::string> const & commandline)
{
    if (commandline.empty())
    {
        throw std::runtime_error("missing command");
    }

    pid = fork();
    if (pid == 0)
    {
        size_t const count = commandline.size() + 1;
        char ** args = reinterpret_cast<char**>(malloc(sizeof(char*) * count));
        args[count - 1] = nullptr;
        for(size_t i = 0; i < commandline.size(); i++)
        {
            args[i] = strdup(commandline[i].c_str());
        }

        closefrom(0);
        open("/dev/null", O_RDONLY);
        open("/dev/null", O_WRONLY);
        dup2(STDOUT_FILENO, STDERR_FILENO);

        execv(args[0], args);

        // this should not be reached
        for(size_t i = 0; i < count; i++)
        {
            free(args[i]);
        }
        free(args);

        exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    {
        // parent: do nothing
    }
    else
    {
        throw std::runtime_error("failed to fork");
    }
}

process::~process()
{
    if (pid > 0)
    {
        wait();
    }
}

void process::kill(int signal_number)
{
    if (pid > 0)
    {
        ::kill(pid, signal_number);
    }
}

int process::wait()
{
    int exit_code = -1;

    if (pid > 0)
    {
        int status = 0;
        int rc = waitpid(pid, &status, 0);
        if (rc == pid)
        {
            exit_code = WEXITSTATUS(status);
            pid = 0;
        }
    }

    return exit_code;
}

}
