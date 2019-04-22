#ifndef WF_PATH_H
#define WF_PATH_H

#ifndef __cplusplus
#include <stddef.h>
#else
#include <cstddef>
using ::std::size_t;
#endif

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_path;

extern struct wf_path *
wf_path_create(
    char const * value);

extern void
wf_path_dispose(
    struct wf_path * path);

extern size_t
wf_path_element_count(
    struct wf_path * path);

extern char const *
wf_path_get_element(
    struct wf_path * path,
    size_t i);

extern char const *
wf_path_get_filename(
    struct wf_path * path);

#ifdef __cplusplus
}
#endif

#endif
