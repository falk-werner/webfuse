#include "webfuse/impl/json/doc.h"
#include "webfuse/impl/json/node_intern.h"
#include "webfuse/impl/json/reader.h"
#include "webfuse/impl/json/parser.h"

#include <stdlib.h>

struct wf_json_doc
{
    struct wf_json root;
};

struct wf_json_doc *
wf_impl_json_doc_loadb(
    char * data,
    size_t length)
{
    struct wf_json_reader reader;
    wf_impl_json_reader_init(&reader, data, length);

    struct wf_json_doc * doc = malloc(sizeof(struct wf_json_doc));
    if (!wf_impl_json_parse_value(&reader, &doc->root))
    {
        free(doc);
        doc = NULL;
    }

    return doc;    
}

void
wf_impl_json_doc_dispose(
    struct wf_json_doc * doc)
{
    wf_impl_json_cleanup(&doc->root);
    free(doc);
}

struct wf_json const *
wf_impl_jsoc_doc_root(
    struct wf_json_doc * doc)
{
    return &doc->root;
}
