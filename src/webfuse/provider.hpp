#ifndef WEBFUSE_PROVIDER_I_HPP
#define WEBFUSE_PROVIDER_I_HPP

#include "webfuse/filesystem/filesystem_i.hpp"
#include <functional>
#include <string>

namespace webfuse
{

class provider
{
    provider(provider const &) = delete;
    provider& operator=(provider const &) = delete;
public:
    provider(filesystem_i & fs, std::string const & ca_path);
    ~provider();
    provider(provider && other);
    provider& operator=(provider && other);
    void set_connection_listener(std::function<void(bool)> listener);
    void connect(std::string const & url);
    void service();
    void interrupt();
private:
    class detail;
    detail * d;
};

}

#endif
