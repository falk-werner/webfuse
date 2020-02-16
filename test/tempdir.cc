#include "webfuse/core/string.h"
#include "tempdir.hpp"

#include <unistd.h>
#include <cstdlib>
#include <stdexcept>

namespace webfuse_test
{

TempDir::TempDir(char const * prefix)
: path_(wf_create_string("/tmp/%s_XXXXXX", prefix))
{
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