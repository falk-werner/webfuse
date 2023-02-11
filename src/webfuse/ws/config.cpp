#include "webfuse/ws/config.hpp"
#include "webfuse/util/commandline_reader.hpp"

#include <cctype>
#include <algorithm>
#include <iostream>

namespace
{

constexpr int const default_port = 8081;
constexpr char const default_vhost_name[] = "localhost";
constexpr uint64_t const default_timeout_secs = 10;

void verify(webfuse::ws_config & config)
{
    if (config.cmd == webfuse::command::run)
    {
        if ((config.use_tls) && ((config.key_path.empty()) || (config.cert_path.empty())))
        {
            std::cerr << "error: use of TLS requires both, key ander certificate path" << std::endl;
            config.cmd = webfuse::command::show_help;
            config.exit_code = EXIT_FAILURE;
        }
    }
}

bool get_arg(webfuse::ws_config & config, webfuse::commandline_reader& reader, std::string & value, std::string const error_message)
{
    const bool has_next = reader.next();
    if (has_next)
    {
        value = reader.current();
    }
    else
    {
        std::cerr << "error: " << error_message << std::endl;
        config.cmd = webfuse::command::show_help;
        config.exit_code = EXIT_FAILURE;
    }

    return has_next;
}

}

namespace webfuse
{

// NOLINTNEXTLINE
ws_config::ws_config(int argc, char * argv[])
: exit_code(EXIT_SUCCESS)
, args(argv[0], argc)
, cmd(command::run)
, port(default_port)
, vhost_name(default_vhost_name)
, use_tls(false)
, timeout_secs(default_timeout_secs)
{
    commandline_reader reader(argc, argv);

    while ((exit_code == EXIT_SUCCESS) && (reader.next()))
    {
        std::string const arg = reader.current();
        if ((arg == "-h") || (arg == "--help"))
        {
            cmd = command::show_help;
        }
        else if (arg == "--wf-port")
        {
            std::string value;
            if (get_arg(*this, reader, value, "missing PORT"))
            {
                port = static_cast<uint16_t>(std::stoi(value));
            }
        }
        else if (arg == "--wf-vhost")
        {
            get_arg(*this, reader, vhost_name, "missing VHOST");
        }
        else if (arg == "--wf-key")
        {
            if (get_arg(*this, reader, key_path, "missing KEY_PATH"))
            {
                use_tls = true;
            }
        }
        else if (arg == "--wf-cert")
        {
            if (get_arg(*this, reader, cert_path, "missing CERT_PATH"))
            {
                use_tls = true;
            }
        }
        else if (arg == "--wf-authenticator")
        {
            get_arg(*this, reader, authenticator, "missing AUTHENTICATOR");
        }
        else if (arg == "--wf-auth-header")
        {
            if (get_arg(*this, reader, auth_header, "missing AUTH_HEADER"))
            {
                std::transform(auth_header.begin(), auth_header.end(), auth_header.begin(),
                    [](auto c) {return std::tolower(c); });
            }
        }
        else if (arg == "--wf-timeout")
        {
            std::string timeout_str;
            if (get_arg(*this, reader, timeout_str, "missing TIMEOUT"))
            {
                timeout_secs = static_cast<uint64_t>(std::stoi(timeout_str));
            }
        }
        else if (arg == "--wf-docroot")
        {
            get_arg(*this, reader, docroot, "missing DOCROOT");
        }
        else if (arg == "--wf-version")
        {
            cmd = command::print_version;
        }
        else
        {
            args.push(arg.c_str());
        }
    }

    verify(*this);
}


}
