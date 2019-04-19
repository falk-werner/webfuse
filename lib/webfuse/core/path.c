#include "webfuse/core/path.h"

struct wf_path *
wf_path_create(
    char const * value)
{
    (void) value;
    return NULL;
}

void
wf_path_dispose(
    struct wf_path * path)
{
    (void) path;
}

size_t
wf_path_element_count(
    struct wf_path * path)
{
    (void) path;
    return 0;
}

char const *
wf_path_get_element(
    struct wf_path * path,
    size_t i)
{  
    (void) path;
    (void) i;
    return NULL;
}
