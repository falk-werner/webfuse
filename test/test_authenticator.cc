#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "mock_authenticator.hpp"

#include "wsfs/adapter/impl/authenticator.h"
#include "wsfs/adapter/impl/credentials.h"

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

    struct credentials creds;
    credentials_init(&creds, "username", nullptr);
    char dummy[] = "usr_data";
    void * user_data = reinterpret_cast<void*>(dummy);

    EXPECT_CALL(mock, authenticate(&creds, user_data))
        .Times(1)
        .WillRepeatedly(Return(true));

    struct authenticator * authenticator = authenticator_create(
        "username",
        &authenticate,
        user_data);

    bool result = authenticator_autenticate(authenticator, &creds);
    ASSERT_TRUE(result);

    authenticator_dispose(authenticator);
    credentials_cleanup(&creds);
}

TEST(Authenticator, SkipAuthenticationWithWrongType)
{
    MockAuthenticator mock;
    set_authenticator(&mock);

    struct credentials creds;
    credentials_init(&creds, "username", nullptr);
    EXPECT_CALL(mock, authenticate(_, _))
        .Times(0);

    struct authenticator * authenticator = authenticator_create(
        "certificate",
        &authenticate,
        nullptr);

    bool result = authenticator_autenticate(authenticator, &creds);
    ASSERT_FALSE(result);

    authenticator_dispose(authenticator);
    credentials_cleanup(&creds);
}