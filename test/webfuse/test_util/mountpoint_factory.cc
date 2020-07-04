#include "webfuse/test_util/mountpoint_factory.hpp"
#include "webfuse/mountpoint.h"

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/stat.h>
#include <string>

extern "C"
{

static void
webfuse_test_cleanup_mountpoint(
    void * user_data)
{
    char * path = reinterpret_cast<char*>(user_data);
    rmdir(path);
    free(path);
}

struct wf_mountpoint *
webfuse_test_create_mountpoint(
    char const * filesystem,
    void * user_data)
{
    char const * base_dir = reinterpret_cast<char const*>(user_data);
    char * path;
    asprintf(&path, "%s/%s", base_dir, filesystem);
    mkdir(path, 0755);
    struct wf_mountpoint * mountpoint = wf_mountpoint_create(path);
    wf_mountpoint_set_userdata(
        mountpoint,
        reinterpret_cast<void*>(path),
        &webfuse_test_cleanup_mountpoint);

    return mountpoint;
}


}