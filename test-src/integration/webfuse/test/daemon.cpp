#include "webfuse/test/daemon.hpp"

#include <limits.h>
#include <unistd.h>
#include <libgen.h>

#include <csignal>

namespace
{

std::string get_executable_path()
{
    char directory[PATH_MAX] = { '\0' };
    readlink("/proc/self/exe", directory, PATH_MAX);
    dirname(directory);

    return std::string(directory) + "/webfuse";
}

}

namespace webfuse
{

daemon::daemon(std::string const & mountpoint)
: p({get_executable_path(), "-f", mountpoint})
{

}

daemon::~daemon()
{
    p.kill(SIGINT);
}

}
