#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "wsfs/adapter/authenticators.h"
#include "wsfs/adapter/credentials_intern.h"
#include "mock_authenticator.hpp"

using ::testing::_;
using ::testing::Return;
using ::wsfs_test::MockAuthenticator;
using ::wsfs_test::set_authenticator;
using ::wsfs_test::authenticate;
using ::wsfs_test::authenticate_1;
using ::wsfs_test::authenticate_2;


TEST(Authenticators, CloneEmpty)
{
    struct wsfs_authenticators authenticators;
    struct wsfs_authenticators clone;

    wsfs_authenticators_init(&authenticators);
    ASSERT_EQ(nullptr, authenticators.first);

    wsfs_authenticators_clone(&authenticators, &clone);
    ASSERT_EQ(nullptr, clone.first);

    wsfs_authenticators_cleanup(&authenticators);
    wsfs_authenticators_cleanup(&clone);
}

TEST(Authenticators, Clone)
{
    struct wsfs_authenticators authenticators;
    struct wsfs_authenticators clone;

    wsfs_authenticators_init(&authenticators);
    wsfs_authenticators_add(&authenticators, "username", &authenticate, nullptr);
    ASSERT_NE(nullptr, authenticators.first);

    wsfs_authenticators_clone(&authenticators, &clone);
    ASSERT_NE(nullptr, clone.first);
    ASSERT_NE(nullptr, authenticators.first);
    ASSERT_NE(authenticators.first, clone.first);

    wsfs_authenticators_cleanup(&authenticators);
    wsfs_authenticators_cleanup(&clone);
}

TEST(Authenticators, Move)
{
    struct wsfs_authenticators authenticators;
    struct wsfs_authenticators clone;

    wsfs_authenticators_init(&authenticators);
    wsfs_authenticators_add(&authenticators, "username", &authenticate, nullptr);
    ASSERT_NE(nullptr, authenticators.first);

    wsfs_authenticators_move(&authenticators, &clone);
    ASSERT_NE(nullptr, clone.first);
    ASSERT_EQ(nullptr, authenticators.first);
    ASSERT_NE(authenticators.first, clone.first);

    wsfs_authenticators_cleanup(&authenticators);
    wsfs_authenticators_cleanup(&clone);
}

TEST(Authenticators, AuthenticateWithoutAuthenticators)
{
    struct wsfs_credentials creds;
    wsfs_credentials_init(&creds, "username", nullptr);

    struct wsfs_authenticators authenticators;
    wsfs_authenticators_init(&authenticators);

    bool result = wsfs_authenticators_authenticate(&authenticators, &creds);
    ASSERT_TRUE(result);

    result = wsfs_authenticators_authenticate(&authenticators, nullptr);
    ASSERT_TRUE(result);

    wsfs_authenticators_cleanup(&authenticators);
    wsfs_credentials_cleanup(&creds);
}

TEST(Authenticators, FailToAuthenticateWithoutCredentials)
{
    MockAuthenticator mock;
    set_authenticator(&mock);

    struct wsfs_authenticators authenticators;
    wsfs_authenticators_init(&authenticators);
    wsfs_authenticators_add(&authenticators, "username", &authenticate, nullptr);

    bool result = wsfs_authenticators_authenticate(&authenticators, nullptr);
    ASSERT_FALSE(result);

    wsfs_authenticators_cleanup(&authenticators);
}

TEST(Authenticators, AuthenticateWithMultipleCredentials)
{
    struct wsfs_credentials creds;
    wsfs_credentials_init(&creds, "username", nullptr);

    MockAuthenticator username_mock;
    set_authenticator(1, &username_mock);
    EXPECT_CALL(username_mock, authenticate(&creds, nullptr))
        .Times(1)
        .WillRepeatedly(Return(true));

    MockAuthenticator certificate_mock;
    set_authenticator(2, &certificate_mock);
    EXPECT_CALL(certificate_mock, authenticate(_, _))
        .Times(0);

    struct wsfs_authenticators authenticators;
    wsfs_authenticators_init(&authenticators);
    wsfs_authenticators_add(&authenticators, "username", &authenticate_1, nullptr);
    wsfs_authenticators_add(&authenticators, "certificate", &authenticate_2, nullptr);

    bool result = wsfs_authenticators_authenticate(&authenticators, &creds);
    ASSERT_TRUE(result);

    wsfs_authenticators_cleanup(&authenticators);
    wsfs_credentials_cleanup(&creds);
}

TEST(Authenticators, FailedAuthenticateWithWrongType)
{
    struct wsfs_credentials creds;
    wsfs_credentials_init(&creds, "token", nullptr);

    MockAuthenticator username_mock;
    set_authenticator(1, &username_mock);
    EXPECT_CALL(username_mock, authenticate(&creds, nullptr))
        .Times(0);

    MockAuthenticator certificate_mock;
    set_authenticator(2, &certificate_mock);
    EXPECT_CALL(certificate_mock, authenticate(_, _))
        .Times(0);

    struct wsfs_authenticators authenticators;
    wsfs_authenticators_init(&authenticators);
    wsfs_authenticators_add(&authenticators, "username", &authenticate_1, nullptr);
    wsfs_authenticators_add(&authenticators, "certificate", &authenticate_2, nullptr);

    bool result = wsfs_authenticators_authenticate(&authenticators, &creds);
    ASSERT_FALSE(result);

    wsfs_authenticators_cleanup(&authenticators);
    wsfs_credentials_cleanup(&creds);
}
