#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "webfuse/adapter/impl/authenticators.h"
#include "webfuse/adapter/impl/credentials.h"
#include "webfuse/mocks/mock_authenticator.hpp"

using ::testing::_;
using ::testing::Return;
using ::webfuse_test::MockAuthenticator;
using ::webfuse_test::set_authenticator;
using ::webfuse_test::authenticate;
using ::webfuse_test::authenticate_1;
using ::webfuse_test::authenticate_2;


TEST(Authenticators, CloneEmpty)
{
    struct wf_impl_authenticators authenticators;
    struct wf_impl_authenticators clone;

    wf_impl_authenticators_init(&authenticators);
    ASSERT_EQ(nullptr, authenticators.first);

    wf_impl_authenticators_clone(&authenticators, &clone);
    ASSERT_EQ(nullptr, clone.first);

    wf_impl_authenticators_cleanup(&authenticators);
    wf_impl_authenticators_cleanup(&clone);
}

TEST(Authenticators, Clone)
{
    struct wf_impl_authenticators authenticators;
    struct wf_impl_authenticators clone;

    wf_impl_authenticators_init(&authenticators);
    wf_impl_authenticators_add(&authenticators, "username", &authenticate, nullptr);
    ASSERT_NE(nullptr, authenticators.first);

    wf_impl_authenticators_clone(&authenticators, &clone);
    ASSERT_NE(nullptr, clone.first);
    ASSERT_NE(nullptr, authenticators.first);
    ASSERT_NE(authenticators.first, clone.first);

    wf_impl_authenticators_cleanup(&authenticators);
    wf_impl_authenticators_cleanup(&clone);
}

TEST(Authenticators, Move)
{
    struct wf_impl_authenticators authenticators;
    struct wf_impl_authenticators clone;

    wf_impl_authenticators_init(&authenticators);
    wf_impl_authenticators_add(&authenticators, "username", &authenticate, nullptr);
    ASSERT_NE(nullptr, authenticators.first);

    wf_impl_authenticators_move(&authenticators, &clone);
    ASSERT_NE(nullptr, clone.first);
    ASSERT_EQ(nullptr, authenticators.first);
    ASSERT_NE(authenticators.first, clone.first);

    wf_impl_authenticators_cleanup(&authenticators);
    wf_impl_authenticators_cleanup(&clone);
}

TEST(Authenticators, AuthenticateWithoutAuthenticators)
{
    struct wf_credentials creds;
    wf_impl_credentials_init(&creds, "username", nullptr);

    struct wf_impl_authenticators authenticators;
    wf_impl_authenticators_init(&authenticators);

    bool result = wf_impl_authenticators_authenticate(&authenticators, &creds);
    ASSERT_TRUE(result);

    result = wf_impl_authenticators_authenticate(&authenticators, nullptr);
    ASSERT_TRUE(result);

    wf_impl_authenticators_cleanup(&authenticators);
    wf_impl_credentials_cleanup(&creds);
}

TEST(Authenticators, FailToAuthenticateWithoutCredentials)
{
    MockAuthenticator mock;
    set_authenticator(&mock);

    struct wf_impl_authenticators authenticators;
    wf_impl_authenticators_init(&authenticators);
    wf_impl_authenticators_add(&authenticators, "username", &authenticate, nullptr);

    bool result = wf_impl_authenticators_authenticate(&authenticators, nullptr);
    ASSERT_FALSE(result);

    wf_impl_authenticators_cleanup(&authenticators);
}

TEST(Authenticators, AuthenticateWithMultipleCredentials)
{
    struct wf_credentials creds;
    wf_impl_credentials_init(&creds, "username", nullptr);

    MockAuthenticator username_mock;
    set_authenticator(1, &username_mock);
    EXPECT_CALL(username_mock, authenticate(&creds, nullptr))
        .Times(1)
        .WillRepeatedly(Return(true));

    MockAuthenticator certificate_mock;
    set_authenticator(2, &certificate_mock);
    EXPECT_CALL(certificate_mock, authenticate(_, _))
        .Times(0);

    struct wf_impl_authenticators authenticators;
    wf_impl_authenticators_init(&authenticators);
    wf_impl_authenticators_add(&authenticators, "username", &authenticate_1, nullptr);
    wf_impl_authenticators_add(&authenticators, "certificate", &authenticate_2, nullptr);

    bool result = wf_impl_authenticators_authenticate(&authenticators, &creds);
    ASSERT_TRUE(result);

    wf_impl_authenticators_cleanup(&authenticators);
    wf_impl_credentials_cleanup(&creds);
}

TEST(Authenticators, FailedAuthenticateWithWrongType)
{
    struct wf_credentials creds;
    wf_impl_credentials_init(&creds, "token", nullptr);

    MockAuthenticator username_mock;
    set_authenticator(1, &username_mock);
    EXPECT_CALL(username_mock, authenticate(&creds, nullptr))
        .Times(0);

    MockAuthenticator certificate_mock;
    set_authenticator(2, &certificate_mock);
    EXPECT_CALL(certificate_mock, authenticate(_, _))
        .Times(0);

    struct wf_impl_authenticators authenticators;
    wf_impl_authenticators_init(&authenticators);
    wf_impl_authenticators_add(&authenticators, "username", &authenticate_1, nullptr);
    wf_impl_authenticators_add(&authenticators, "certificate", &authenticate_2, nullptr);

    bool result = wf_impl_authenticators_authenticate(&authenticators, &creds);
    ASSERT_FALSE(result);

    wf_impl_authenticators_cleanup(&authenticators);
    wf_impl_credentials_cleanup(&creds);
}
