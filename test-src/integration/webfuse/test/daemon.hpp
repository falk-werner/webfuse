#ifndef WEBFUSE_DAEMOM_HPP
#define WEBFUSE_DAEMOM_HPP

#include "webfuse/test/process.hpp"
#include <string>

namespace webfuse
{

class daemon
{
    daemon(daemon const &) = delete;
    daemon& operator=(daemon const &) = delete;
    daemon(daemon &&) = delete;
    daemon& operator=(daemon &&) = delete;
public:
    explicit daemon(std::string const & mountpoint);
    ~daemon();
private:
    process p;
};

}

#endif
