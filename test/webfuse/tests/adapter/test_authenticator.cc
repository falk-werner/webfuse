#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "webfuse/mocks/mock_authenticator.hpp"

#include "webfuse/adapter/impl/authenticator.h"
#include "webfuse/adapter/impl/credentials.h"

using ::testing::Return;
using ::testing::_;
using ::webfuse_test::Authenticator;
using ::webfuse_test::MockAuthenticator;
using ::webfuse_test::set_authenticator;
using ::webfuse_test::authenticate;


TEST(Authenticator, Authenticate)
{
    MockAuthenticator mock;
    set_authenticator(&mock);

    struct wf_credentials creds;
    wf_impl_credentials_init(&creds, "username", nullptr);
    char dummy[] = "usr_data";
    void * user_data = reinterpret_cast<void*>(dummy);

    EXPECT_CALL(mock, authenticate(&creds, user_data))
        .Times(1)
        .WillRepeatedly(Return(true));

    struct wf_impl_authenticator * authenticator = wf_impl_authenticator_create(
        "username",
        &authenticate,
        user_data);

    bool result = wf_impl_authenticator_autenticate(authenticator, &creds);
    ASSERT_TRUE(result);

    wf_impl_authenticator_dispose(authenticator);
    wf_impl_credentials_cleanup(&creds);
}

TEST(Authenticator, SkipAuthenticationWithWrongType)
{
    MockAuthenticator mock;
    set_authenticator(&mock);

    struct wf_credentials creds;
    wf_impl_credentials_init(&creds, "username", nullptr);
    EXPECT_CALL(mock, authenticate(_, _))
        .Times(0);

    struct wf_impl_authenticator * authenticator = wf_impl_authenticator_create(
        "certificate",
        &authenticate,
        nullptr);

    bool result = wf_impl_authenticator_autenticate(authenticator, &creds);
    ASSERT_FALSE(result);

    wf_impl_authenticator_dispose(authenticator);
    wf_impl_credentials_cleanup(&creds);
}