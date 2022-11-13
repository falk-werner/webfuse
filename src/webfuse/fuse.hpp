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
    fuse (fuse &&) = delete;
    fuse& operator=(fuse &&) = delete;
    void run(int argc, char * argv[]);
private:
    class detail;
    detail * d;
};

}

#endif
