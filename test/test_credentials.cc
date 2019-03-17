#include <gtest/gtest.h>

#include "wsfs/adapter/credentials_intern.h"
#include <jansson.h>

TEST(Credentials, Type)
{
    struct wsfs_credentials creds;

    wsfs_credentials_init(&creds, "test", nullptr);
    ASSERT_STREQ("test", wsfs_credentials_type(&creds));
    wsfs_credentials_cleanup(&creds);
}

TEST(Credentials, Get)
{
    struct wsfs_credentials creds;
    json_t * data = json_object();
    json_object_set_new(data, "username", json_string("bob"));
    json_object_set_new(data, "password", json_string("<secret>"));

    wsfs_credentials_init(&creds, "username", data);
    ASSERT_STREQ("username", wsfs_credentials_type(&creds));
    ASSERT_STREQ("bob", wsfs_credentials_get(&creds, "username"));
    ASSERT_STREQ("<secret>", wsfs_credentials_get(&creds, "password"));

    wsfs_credentials_cleanup(&creds);
    json_decref(data);
}

TEST(Credentials, FailedToGetNonexistingValue)
{
    struct wsfs_credentials creds;
    json_t * data = json_object();

    wsfs_credentials_init(&creds, "username", data);
    ASSERT_STREQ("username", wsfs_credentials_type(&creds));
    ASSERT_STREQ(nullptr, wsfs_credentials_get(&creds, "username"));
    ASSERT_STREQ(nullptr, wsfs_credentials_get(&creds, "password"));

    wsfs_credentials_cleanup(&creds);
    json_decref(data);    
}

TEST(Credentials, FailedToGetWithoutData)
{
    struct wsfs_credentials creds;

    wsfs_credentials_init(&creds, "username", nullptr);
    ASSERT_STREQ("username", wsfs_credentials_type(&creds));
    ASSERT_STREQ(nullptr, wsfs_credentials_get(&creds, "username"));
    ASSERT_STREQ(nullptr, wsfs_credentials_get(&creds, "password"));

    wsfs_credentials_cleanup(&creds);
}

TEST(Credentials, FailedToGetWrongDataType)
{
    struct wsfs_credentials creds;
    json_t * data = json_string("invalid_creds");

    wsfs_credentials_init(&creds, "username", data);
    ASSERT_STREQ("username", wsfs_credentials_type(&creds));
    ASSERT_STREQ(nullptr, wsfs_credentials_get(&creds, "username"));
    ASSERT_STREQ(nullptr, wsfs_credentials_get(&creds, "password"));

    wsfs_credentials_cleanup(&creds);
    json_decref(data);    
}

