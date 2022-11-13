#include "webfuse/webfuse.hpp"
#include "webfuse/fuse.hpp"
#include "webfuse/filesystem/empty_filesystem.hpp"
#include "webfuse/ws/server.hpp"
#include "webfuse/ws/config.hpp"

#include <csignal>

namespace
{

bool shutdown_requested = false;

void on_shutdown_requested(int)
{
    shutdown_requested = true;
}

}

namespace webfuse
{

int app::run(int argc, char * argv[])
{
/*
    empty_filesystem filesystem;
    fuse fuse_fs(filesystem);

    return fuse_fs.run(argc, argv);
*/
    signal(SIGINT, &on_shutdown_requested);

    ws_config config;
    ws_server server(config);

    while (!shutdown_requested)
    {
        server.service();
    }

    return 0;
}

}