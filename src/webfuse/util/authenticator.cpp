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

    int fds[2];
    int const rc = pipe(fds);
    if (0 != rc)
    {
        return false;
    }

    pid_t const pid = fork();

    if (pid == 0)
    {
        // child

        close(STDIN_FILENO);
        dup2(fds[0], STDIN_FILENO);

        // prepare file descriptors
        closefrom(1);
        open("/dev/null", O_WRONLY);
        dup2(STDOUT_FILENO, STDERR_FILENO);

        execl(app_.c_str(), app_.c_str(), nullptr);
        exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    {
        write(fds[1], reinterpret_cast<void const*>(token.c_str()), token.size());
        close(fds[1]);

        // parent
        int exit_status = EXIT_FAILURE;

        int status = 0;
        int const rc = waitpid(pid, &status, 0);
        if (rc == pid)
        {
            exit_status = WEXITSTATUS(status);
        }

        close(fds[0]);
        result = (exit_status == EXIT_SUCCESS);        
    }

    return result;
}


}
