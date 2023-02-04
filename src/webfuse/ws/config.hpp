#ifndef WEBFUSE_WS_CONFIG_HPP
#define WEBFUSE_WS_CONFIG_HPP

#include <webfuse/util/commandline_args.hpp>

#include <cinttypes>
#include <string>

namespace webfuse
{

enum class command
{
    run,
    print_version,
    show_help
};

class ws_config
{
public:
    ws_config(int argc, char * argv[]);

    int exit_code;
    commandline_args args;
    command cmd;

    uint16_t port;
    std::string vhost_name;

    bool use_tls;
    std::string cert_path;
    std::string key_path;

    std::string authenticator;
    std::string auth_header;

    uint64_t timeout_secs;
};

}

#endif
