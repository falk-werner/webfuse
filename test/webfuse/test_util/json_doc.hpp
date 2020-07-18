#ifndef WF_TEST_UTIL_JSON_DOC_HPP
#define WF_TEST_UTIL_JSON_DOC_HPP

#include "webfuse/impl/json/doc.h"
#include <string>

namespace webfuse_test
{

class JsonDoc
{
    JsonDoc(JsonDoc const&) = delete;
    JsonDoc& operator=(JsonDoc const&) = delete;
public:
    JsonDoc(std::string const & text);
    JsonDoc(JsonDoc && other);
    JsonDoc& operator=(JsonDoc && other);
    ~JsonDoc();
    wf_json const * root();
private:
    std::string contents;
    wf_json_doc * doc;
};

}

#endif
