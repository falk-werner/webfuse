#include <gtest/gtest.h>

#include "webfuse/adapter/impl/credentials.h"
#include <jansson.h>

TEST(Credentials, Type)
{
    struct wf_credentials creds;

    wf_impl_credentials_init(&creds, "test", nullptr);
    ASSERT_STREQ("test", wf_impl_credentials_type(&creds));
    wf_impl_credentials_cleanup(&creds);
}

TEST(Credentials, Get)
{
    struct wf_credentials creds;
    json_t * data = json_object();
    json_object_set_new(data, "username", json_string("bob"));
    json_object_set_new(data, "password", json_string("<secret>"));

    wf_impl_credentials_init(&creds, "username", data);
    ASSERT_STREQ("username", wf_impl_credentials_type(&creds));
    ASSERT_STREQ("bob", wf_impl_credentials_get(&creds, "username"));
    ASSERT_STREQ("<secret>", wf_impl_credentials_get(&creds, "password"));

    wf_impl_credentials_cleanup(&creds);
    json_decref(data);
}

TEST(Credentials, FailedToGetNonexistingValue)
{
    struct wf_credentials creds;
    json_t * data = json_object();

    wf_impl_credentials_init(&creds, "username", data);
    ASSERT_STREQ("username", wf_impl_credentials_type(&creds));
    ASSERT_STREQ(nullptr, wf_impl_credentials_get(&creds, "username"));
    ASSERT_STREQ(nullptr, wf_impl_credentials_get(&creds, "password"));

    wf_impl_credentials_cleanup(&creds);
    json_decref(data);    
}

TEST(Credentials, FailedToGetWithoutData)
{
    struct wf_credentials creds;

    wf_impl_credentials_init(&creds, "username", nullptr);
    ASSERT_STREQ("username", wf_impl_credentials_type(&creds));
    ASSERT_STREQ(nullptr, wf_impl_credentials_get(&creds, "username"));
    ASSERT_STREQ(nullptr, wf_impl_credentials_get(&creds, "password"));

    wf_impl_credentials_cleanup(&creds);
}

TEST(Credentials, FailedToGetWrongDataType)
{
    struct wf_credentials creds;
    json_t * data = json_string("invalid_creds");

    wf_impl_credentials_init(&creds, "username", data);
    ASSERT_STREQ("username", wf_impl_credentials_type(&creds));
    ASSERT_STREQ(nullptr, wf_impl_credentials_get(&creds, "username"));
    ASSERT_STREQ(nullptr, wf_impl_credentials_get(&creds, "password"));

    wf_impl_credentials_cleanup(&creds);
    json_decref(data);    
}

