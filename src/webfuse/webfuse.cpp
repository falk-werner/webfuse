#include "webfuse/webfuse.hpp"
#include "webfuse/fuse.hpp"
#include "webfuse/filesystem.hpp"
#include "webfuse/ws/server.hpp"
#include "webfuse/version.hpp"

#include <iostream>

namespace webfuse
{

int app::run(int argc, char * argv[]) // NOLINT(readability-convert-member-functions-to-static)
{
    ws_config config(argc, argv);

    switch (config.cmd)
    {
        case command::run:
            {
                ws_server server(config);
                filesystem filesystem(server);
                fuse fuse_fs(filesystem);

                config.exit_code = fuse_fs.run(config.args.get_argc(), config.args.get_argv());
            }
            break;
        case command::print_version:
            std::cout << webfuse::get_version() << std::endl;
            break;
        case command::show_help:
            // fall-through
        default:
            {
                fuse::print_usage();
                std::cout << R"(
WEBFUSE options:
    --wf-port PORT              port number of websocket server (default: 8081)
    --wf-vhost VHOST            name of the virtual host (default: localhost)
    --wf-cert PATH              path of the server's public certificate (optional)
    --wf-key  PATH              path of the server's private key (optional)
    --wf-authenticator PATH     path of authenticatior app (optional)
    --wf-auth-header   NAME     name of the authentication header (optional)
    --wf-docroot       PATH     enables HTTP server with given document root (optional)
    --wf-timeout       TIMEOUT  communication timeout in seconds (default: 10)
    --wf-version                print version and exit
)";
            }
            break;
    }

    return config.exit_code;
}

}
