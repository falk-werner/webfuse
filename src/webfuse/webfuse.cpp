#include "webfuse/webfuse.hpp"
#include "webfuse/fuse.hpp"
#include "webfuse/filesystem/empty_filesystem.hpp"

namespace webfuse
{

int app::run(int argc, char * argv[])
{
    empty_filesystem filesystem;
    fuse fuse_fs(filesystem);

    return fuse_fs.run(argc, argv);
}

}