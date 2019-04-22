#include <gtest/gtest.h>

#include "webfuse/provider/impl/static_filesystem.h"
#include "webfuse/provider/client_config.h"
#include "webfuse/provider/impl/client_config.h"

#include "mock_request.hpp"

using webfuse_test::request_create;
using webfuse_test::MockRequest;
using webfuse_test::GetAttrMatcher;
using webfuse_test::ReaddirMatcher;
using testing::_;

TEST(wfp_static_filesystem, has_root_dir)
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

TEST(wfp_static_filesystem, contains_default_dirs)
{
    struct wfp_client_config * config = wfp_client_config_create();
    struct wfp_static_filesystem * filesystem = wfp_impl_static_filesystem_create(config);

    MockRequest mock;
    struct wfp_request * request = request_create(&mock, 23);
    char const * default_dirs[] = {".", "..", nullptr};
    EXPECT_CALL(mock, respond(ReaddirMatcher(default_dirs), 23)).Times(1);

    config->provider.readdir(request, 1, config->user_data);

    wfp_impl_static_filesystem_dispose(filesystem);
    wfp_client_config_dispose(config);
}

TEST(wfp_static_filesystem, add_text)
{
    struct wfp_client_config * config = wfp_client_config_create();
    struct wfp_static_filesystem * filesystem = wfp_impl_static_filesystem_create(config);
    wfp_impl_static_filesystem_add_text(filesystem, "text.file", 666, "some text");

    MockRequest mock;
    struct wfp_request * request = request_create(&mock, 23);
    char const * contained_elements[] = {"text.file", nullptr};
    EXPECT_CALL(mock, respond(ReaddirMatcher(contained_elements), 23)).Times(1);

    config->provider.readdir(request, 1, config->user_data);

    wfp_impl_static_filesystem_dispose(filesystem);
    wfp_client_config_dispose(config);
}
