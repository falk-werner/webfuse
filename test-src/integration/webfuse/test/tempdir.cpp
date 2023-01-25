#include "webfuse/test/tempdir.hpp"
#include <unistd.h>

namespace webfuse
{

tempdir::tempdir()
{
    char path_template[] = "/tmp/webfuse_test_XXXXXX";
    path = mkdtemp(path_template);
}

tempdir::~tempdir()
{
    rmdir(path.c_str());
}

std::string const & tempdir::name() const
{
    return path;
}

}
