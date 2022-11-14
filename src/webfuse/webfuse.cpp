#include "webfuse/webfuse.hpp"
#include "webfuse/fuse.hpp"
#include "webfuse/filesystem/empty_filesystem.hpp"
#include "webfuse/ws/server.hpp"

namespace webfuse
{

int app::run(int argc, char * argv[])
{
    empty_filesystem filesystem;
    fuse fuse_fs(filesystem);
    ws_config config;
    ws_server server(config);

    return fuse_fs.run(argc, argv);
}

}