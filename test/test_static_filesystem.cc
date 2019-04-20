#include <gtest/gtest.h>

#include "webfuse/provider/impl/static_filesystem.h"
#include "webfuse/provider/client_config.h"
#include "webfuse/provider/impl/client_config.h"

#include "mock_request.hpp"

using webfuse_test::request_create;
using webfuse_test::MockRequest;
using webfuse_test::GetAttrMatcher;
using testing::_;

TEST(wfp_static_filesystem, init)
{
    struct wfp_client_config * config = wfp_client_config_create();
    struct wfp_static_filesystem * filesystem = wfp_impl_static_filesystem_create(config);

    MockRequest mock;
    struct wfp_request * request = request_create(&mock, 42);

    EXPECT_CALL(mock, respond(GetAttrMatcher(1, 0555, "dir"), 42)).Times(1);

    config->provider.getattr(request, 1, config->user_data);



    wfp_impl_static_filesystem_dispose(filesystem);
    wfp_client_config_dispose(config);
}
