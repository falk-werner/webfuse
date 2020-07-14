#ifndef WF_IMPL_JSON_DOC_H
#define WF_IMPL_JSON_DOC_H

#ifndef __cplusplus
#include <stddef.h>
#else
#include <cstddef>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_json_doc;
struct wf_json;

extern struct wf_json_doc *
wf_impl_json_doc_loadb(
    char * data,
    size_t length);

extern void
wf_impl_json_doc_dispose(
    struct wf_json_doc * doc);

extern struct wf_json const *
wf_impl_jsoc_doc_root(
    struct wf_json_doc * doc);


#ifdef __cplusplus
}
#endif

#endif
