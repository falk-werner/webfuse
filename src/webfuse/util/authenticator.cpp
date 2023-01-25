#include "webfuse/util/authenticator.hpp"

#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

namespace webfuse
{

authenticator::authenticator(std::string const & app)
: app_(app)
{

}

bool authenticator::authenticate(std::string const & token)
{
    bool result = false;

    pid_t const pid = fork();

    if (pid == 0)
    {
        // child

        // prepare file descriptors
        closefrom(0);
        open("/dev/null", O_RDONLY);
        open("/dev/null", O_WRONLY);
        dup2(STDOUT_FILENO, STDERR_FILENO);

        execl(app_.c_str(), app_.c_str(), token.c_str(), nullptr);
        exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    {
        // parent
        int exit_status = EXIT_FAILURE;

        int status = 0;
        int const rc = waitpid(pid, &status, 0);
        if (rc == pid)
        {
            exit_status = WEXITSTATUS(status);
        }

        result = (exit_status == EXIT_SUCCESS);
    }

    return result;
}


}
