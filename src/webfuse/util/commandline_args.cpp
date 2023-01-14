#include "webfuse/util/commandline_args.hpp"
#include <cstring>
#include <stdexcept>

namespace webfuse
{

commandline_args::commandline_args(char const * prog_name, int capacity)
: capacity_(capacity)
, argc(0)
{
    if (capacity < 1)
    {
        throw std::runtime_error("too few commandline args");
    }

    argv = new char*[capacity_ + 1];
    argv[0] = nullptr;
    push(prog_name);
}

commandline_args::~commandline_args()
{
    for(int i = 0; i < argc; i++)
    {
        free(argv[i]);
    }
    delete[] argv;
}

void commandline_args::push(char const * arg)
{
    if (argc < capacity_)
    {
        argv[argc] = strdup(arg);
        argv[argc + 1] = nullptr;
        argc++;
    }
    else
    {
        throw std::runtime_error("capacity exceeded");
    }
}

int commandline_args::get_argc() const
{
    return argc;
}

char ** commandline_args::get_argv()
{
    return argv;
}


}