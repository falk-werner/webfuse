#ifndef WF_TEST_UTIL_JSON_DOC_HPP
#define WF_TEST_UTIL_JSON_DOC_HPP

#include "webfuse/impl/json/doc.h"
#include <string>

namespace webfuse_test
{

class JsonDoc
{
public:
    JsonDoc(std::string const & text);
    ~JsonDoc();
    wf_json const * root();
private:
    std::string contents;
    wf_json_doc * doc;
};

}

#endif
