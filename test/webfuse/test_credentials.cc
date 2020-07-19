#include "webfuse/credentials.h"
#include "webfuse/impl/credentials.h"
#include "webfuse/test_util/json_doc.hpp"

#include <gtest/gtest.h>

using webfuse_test::JsonDoc;

TEST(Credentials, Type)
{
    struct wf_credentials creds;

    wf_impl_credentials_init(&creds, "test", nullptr);
    ASSERT_STREQ("test", wf_credentials_type(&creds));
    wf_impl_credentials_cleanup(&creds);
}

TEST(Credentials, Get)
{
    struct wf_credentials creds;
    JsonDoc doc("{\"username\": \"bob\", \"password\": \"<secret>\"}");

    wf_impl_credentials_init(&creds, "username", doc.root());
    ASSERT_STREQ("username", wf_credentials_type(&creds));
    ASSERT_STREQ("bob", wf_credentials_get(&creds, "username"));
    ASSERT_STREQ("<secret>", wf_credentials_get(&creds, "password"));

    wf_impl_credentials_cleanup(&creds);
}

TEST(Credentials, FailedToGetNonexistingValue)
{
    struct wf_credentials creds;
    JsonDoc doc("{}");

    wf_impl_credentials_init(&creds, "username", doc.root());
    ASSERT_STREQ("username", wf_credentials_type(&creds));
    ASSERT_STREQ(nullptr, wf_credentials_get(&creds, "username"));
    ASSERT_STREQ(nullptr, wf_credentials_get(&creds, "password"));

    wf_impl_credentials_cleanup(&creds);
}

TEST(Credentials, FailedToGetWithoutData)
{
    struct wf_credentials creds;

    wf_impl_credentials_init(&creds, "username", nullptr);
    ASSERT_STREQ("username", wf_credentials_type(&creds));
    ASSERT_STREQ(nullptr, wf_credentials_get(&creds, "username"));
    ASSERT_STREQ(nullptr, wf_credentials_get(&creds, "password"));

    wf_impl_credentials_cleanup(&creds);
}

TEST(Credentials, FailedToGetWrongDataType)
{
    struct wf_credentials creds;
    JsonDoc doc("invalid_creds");

    wf_impl_credentials_init(&creds, "username", doc.root());
    ASSERT_STREQ("username", wf_credentials_type(&creds));
    ASSERT_STREQ(nullptr, wf_credentials_get(&creds, "username"));
    ASSERT_STREQ(nullptr, wf_credentials_get(&creds, "password"));

    wf_impl_credentials_cleanup(&creds);
}

TEST(Credentials, FailedToGetWrongElementDataType)
{
    struct wf_credentials creds;
    JsonDoc doc("{\"username\": 42}");

    wf_impl_credentials_init(&creds, "username", doc.root());
    ASSERT_STREQ("username", wf_credentials_type(&creds));
    ASSERT_STREQ(nullptr, wf_credentials_get(&creds, "username"));
    ASSERT_STREQ(nullptr, wf_credentials_get(&creds, "password"));

    wf_impl_credentials_cleanup(&creds);
}

TEST(Credentials, SetType)
{
    struct wf_credentials creds;
    wf_impl_credentials_init_default(&creds);

    wf_credentials_set_type(&creds, "username");
    ASSERT_STREQ("username", wf_credentials_type(&creds));

    wf_impl_credentials_cleanup(&creds);
}

TEST(Credentials, Add)
{
    struct wf_credentials creds;
    wf_impl_credentials_init_default(&creds);

    wf_credentials_add(&creds, "a.value", "a");
    ASSERT_STREQ("a", wf_credentials_get(&creds, "a.value"));

    wf_credentials_add(&creds, "b.value", "b");
    ASSERT_STREQ("b", wf_credentials_get(&creds, "b.value"));

    wf_credentials_add(&creds, "a.value", "A");
    ASSERT_STREQ("A", wf_credentials_get(&creds, "a.value"));

    wf_impl_credentials_cleanup(&creds);
}
