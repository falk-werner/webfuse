#ifndef WEBFUSE_COMMANDLINE_ARGS_HPP
#define WEBFUSE_COMMANDLINE_ARGS_HPP

namespace webfuse
{

class commandline_args
{
    commandline_args (commandline_args const &) = delete;
    commandline_args& operator=(commandline_args const &) = delete;
    commandline_args (commandline_args &&) = delete;
    commandline_args& operator=(commandline_args &&) = delete;
public:
    commandline_args(char const * prog_name, int capacity);
    ~commandline_args();

    void push(char const * arg);
    int get_argc() const;
    char ** get_argv();

private:
    int capacity_;
    int argc;
    char ** argv;
};

}

#endif
