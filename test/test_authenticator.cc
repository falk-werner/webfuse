#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "mock_authenticator.hpp"

#include "wsfs/adapter/authenticator.h"
#include "wsfs/adapter/credentials_intern.h"

using ::testing::Return;
using ::testing::_;
using ::wsfs_test::Authenticator;
using ::wsfs_test::MockAuthenticator;
using ::wsfs_test::set_authenticator;
using ::wsfs_test::authenticate;


TEST(Authenticator, Authenticate)
{
    MockAuthenticator mock;
    set_authenticator(&mock);

    struct wsfs_credentials creds;
    wsfs_credentials_init(&creds, "username", nullptr);
    char dummy[] = "usr_data";
    void * user_data = reinterpret_cast<void*>(dummy);

    EXPECT_CALL(mock, authenticate(&creds, user_data))
        .Times(1)
        .WillRepeatedly(Return(true));

    struct wsfs_authenticator * authenticator = wsfs_authenticator_create(
        "username",
        &authenticate,
        user_data);

    bool result = wsfs_authenticator_autenticate(authenticator, &creds);
    ASSERT_TRUE(result);

    wsfs_authenticator_dispose(authenticator);
    wsfs_credentials_cleanup(&creds);
}

TEST(Authenticator, SkipAuthenticationWithWrongType)
{
    MockAuthenticator mock;
    set_authenticator(&mock);

    struct wsfs_credentials creds;
    wsfs_credentials_init(&creds, "username", nullptr);
    EXPECT_CALL(mock, authenticate(_, _))
        .Times(0);

    struct wsfs_authenticator * authenticator = wsfs_authenticator_create(
        "certificate",
        &authenticate,
        nullptr);

    bool result = wsfs_authenticator_autenticate(authenticator, &creds);
    ASSERT_FALSE(result);

    wsfs_authenticator_dispose(authenticator);
    wsfs_credentials_cleanup(&creds);
}