#include "webfuse/webfuse.hpp"
#include "webfuse/fuse.hpp"
#include "webfuse/filesystem.hpp"
#include "webfuse/ws/server.hpp"

namespace webfuse
{

int app::run(int argc, char * argv[]) // NOLINT(readability-convert-member-functions-to-static)
{
    ws_config config;
    ws_server server(config);
    filesystem filesystem(server);
    fuse fuse_fs(filesystem);

    return fuse_fs.run(argc, argv);
}

}