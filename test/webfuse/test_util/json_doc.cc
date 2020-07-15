#include "webfuse/test_util/json_doc.hpp"

namespace webfuse_test
{

JsonDoc::JsonDoc(std::string const & text)
: contents(text)
{
    doc = wf_impl_json_doc_loadb(const_cast<char*>(contents.data()), contents.size());
}

JsonDoc::~JsonDoc()
{
    wf_impl_json_doc_dispose(doc);
}

wf_json const * JsonDoc::root()
{
    return wf_impl_json_doc_root(doc);
}

}