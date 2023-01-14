#ifndef WEBFUSE_FUSE_HPP
#define WEBFUSE_FUSE_HPP

#include "webfuse/filesystem/filesystem_i.hpp"

namespace webfuse
{

class fuse
{
    fuse (fuse const &) = delete;
    fuse& operator=(fuse const &) = delete;
public:
    explicit fuse(filesystem_i & filesystem);
    ~fuse();
    fuse (fuse && other);
    fuse& operator=(fuse && other);
    int run(int argc, char * argv[]);
    static void print_usage();
private:
    class detail;
    detail * d;
};

}

#endif
