#include <gtest/gtest.h>

#include "wsfs/adapter/impl/credentials.h"
#include <jansson.h>

TEST(Credentials, Type)
{
    struct credentials creds;

    credentials_init(&creds, "test", nullptr);
    ASSERT_STREQ("test", credentials_type(&creds));
    credentials_cleanup(&creds);
}

TEST(Credentials, Get)
{
    struct credentials creds;
    json_t * data = json_object();
    json_object_set_new(data, "username", json_string("bob"));
    json_object_set_new(data, "password", json_string("<secret>"));

    credentials_init(&creds, "username", data);
    ASSERT_STREQ("username", credentials_type(&creds));
    ASSERT_STREQ("bob", credentials_get(&creds, "username"));
    ASSERT_STREQ("<secret>", credentials_get(&creds, "password"));

    credentials_cleanup(&creds);
    json_decref(data);
}

TEST(Credentials, FailedToGetNonexistingValue)
{
    struct credentials creds;
    json_t * data = json_object();

    credentials_init(&creds, "username", data);
    ASSERT_STREQ("username", credentials_type(&creds));
    ASSERT_STREQ(nullptr, credentials_get(&creds, "username"));
    ASSERT_STREQ(nullptr, credentials_get(&creds, "password"));

    credentials_cleanup(&creds);
    json_decref(data);    
}

TEST(Credentials, FailedToGetWithoutData)
{
    struct credentials creds;

    credentials_init(&creds, "username", nullptr);
    ASSERT_STREQ("username",credentials_type(&creds));
    ASSERT_STREQ(nullptr, credentials_get(&creds, "username"));
    ASSERT_STREQ(nullptr, credentials_get(&creds, "password"));

    credentials_cleanup(&creds);
}

TEST(Credentials, FailedToGetWrongDataType)
{
    struct credentials creds;
    json_t * data = json_string("invalid_creds");

    credentials_init(&creds, "username", data);
    ASSERT_STREQ("username", credentials_type(&creds));
    ASSERT_STREQ(nullptr, credentials_get(&creds, "username"));
    ASSERT_STREQ(nullptr, credentials_get(&creds, "password"));

    credentials_cleanup(&creds);
    json_decref(data);    
}

