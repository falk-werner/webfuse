#ifndef WEBFUSE_PROCESS_HPP
#define WEBFUSE_PROCESS_HPP

#include <unistd.h>

#include <string>
#include <vector>

namespace webfuse
{

class process
{
    process(process const &) = delete;
    process operator=(process const &) = delete;
    process(process &&) = delete;
    process operator=(process &&) = delete;
public:
    process(std::vector<std::string> const & commandline);
    ~process();
    void kill(int signal_number);
    int wait();
private:
    pid_t pid;
};

}

#endif
