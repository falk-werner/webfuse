#include "webfuse/impl/json/doc.h"
#include "webfuse/impl/json/node.h"
#include <gtest/gtest.h>

TEST(json_doc, loadb)
{
    char text[] = "true";
    wf_json_doc * doc = wf_impl_json_doc_loadb(text, 4);
    ASSERT_NE(nullptr, doc);

    wf_json const * root = wf_impl_jsoc_doc_root(doc);
    ASSERT_EQ(WF_JSON_TYPE_BOOL, wf_impl_json_type(root));
    ASSERT_TRUE(wf_impl_json_bool_get(root));

    wf_impl_json_doc_dispose(doc);
}

TEST(json_doc, loadb_fail_invalid_json)
{
    char text[] = "true";
    wf_json_doc * doc = wf_impl_json_doc_loadb(text, 3);
    ASSERT_EQ(nullptr, doc);
}