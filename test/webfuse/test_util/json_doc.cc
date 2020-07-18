#include "webfuse/test_util/json_doc.hpp"

namespace webfuse_test
{

JsonDoc::JsonDoc(std::string const & text)
: contents(text)
{
    doc = wf_impl_json_doc_loadb(const_cast<char*>(contents.data()), contents.size());
}

JsonDoc::JsonDoc(JsonDoc && other)
{
    contents = std::move(other.contents);
    doc = other.doc;
    other.doc = nullptr;
}

JsonDoc& JsonDoc::operator=(JsonDoc && other)
{
    if (this != &other)
    {
        wf_impl_json_doc_dispose(doc);
        contents = std::move(other.contents);
        doc = other.doc;
        other.doc = nullptr;
    }

    return *this;
}

JsonDoc::~JsonDoc()
{
    if (nullptr != doc)
    {
        wf_impl_json_doc_dispose(doc);
    }
}

wf_json const * JsonDoc::root()
{
    return wf_impl_json_doc_root(doc);
}

}