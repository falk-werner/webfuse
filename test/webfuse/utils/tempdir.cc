#include "webfuse/utils/tempdir.hpp"

#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>

namespace webfuse_test
{

TempDir::TempDir(char const * prefix)
{
    asprintf(&path_, "/tmp/%s_XXXXXX", prefix);
    char * result = mkdtemp(path_);
    if (NULL == result)
    {
        throw std::runtime_error("unable to create temp dir");
    }
}

TempDir::~TempDir()
{
    rmdir(path_);
    free(path_);
}

char const * TempDir::path()
{
    return path_;
}


}