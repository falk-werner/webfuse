#include <gtest/gtest.h>

#include "webfuse/provider/impl/url.h"

TEST(url, ParseWs)
{
    struct wfp_impl_url url;
    bool result = wfp_impl_url_init(&url, "ws://localhost/");
    ASSERT_TRUE(result);
    ASSERT_EQ(80, url.port);
    ASSERT_FALSE(url.use_tls);
    ASSERT_STREQ("localhost", url.host);
    ASSERT_STREQ("/", url.path);

    wfp_impl_url_cleanup(&url);
}

TEST(url, ParswWss)
{
    struct wfp_impl_url url;
    bool result = wfp_impl_url_init(&url, "wss://localhost/");
    ASSERT_TRUE(result);
    ASSERT_EQ(443, url.port);
    ASSERT_TRUE(url.use_tls);
    ASSERT_STREQ("localhost", url.host);
    ASSERT_STREQ("/", url.path);

    wfp_impl_url_cleanup(&url);
}

TEST(url, ParseIPAdress)
{
    struct wfp_impl_url url;
    bool result = wfp_impl_url_init(&url, "ws://127.0.0.1/");
    ASSERT_TRUE(result);
    ASSERT_EQ(80, url.port);
    ASSERT_STREQ("127.0.0.1", url.host);
    ASSERT_STREQ("/", url.path);

    wfp_impl_url_cleanup(&url);
}

TEST(url, ParsePort)
{
    struct wfp_impl_url url;
    bool result = wfp_impl_url_init(&url, "ws://localhost:54321/");
    ASSERT_TRUE(result);
    ASSERT_EQ(54321, url.port);

    wfp_impl_url_cleanup(&url);
}

TEST(url, ParseNonEmptyPath)
{
    struct wfp_impl_url url;
    bool result = wfp_impl_url_init(&url, "ws://localhost/some_path?query");
    ASSERT_TRUE(result);
    ASSERT_STREQ("/some_path?query", url.path);

    wfp_impl_url_cleanup(&url);
}

TEST(url, FailToParseUnknownProtocol)
{
    struct wfp_impl_url url;
    bool result = wfp_impl_url_init(&url, "unknown://localhost/");
    ASSERT_FALSE(result);
    ASSERT_EQ(0, url.port);
    ASSERT_EQ(nullptr, url.path);
    ASSERT_EQ(nullptr, url.host);
}

TEST(url, FailToParseMissingProtocol)
{
    struct wfp_impl_url url;
    bool result = wfp_impl_url_init(&url, "unknown");
    ASSERT_FALSE(result);
    ASSERT_EQ(0, url.port);
    ASSERT_EQ(nullptr, url.path);
    ASSERT_EQ(nullptr, url.host);
}

TEST(url, FailToParseMissingPath)
{
    struct wfp_impl_url url;
    bool result = wfp_impl_url_init(&url, "ws://localhost");
    ASSERT_FALSE(result);
    ASSERT_EQ(0, url.port);
    ASSERT_EQ(nullptr, url.path);
    ASSERT_EQ(nullptr, url.host);
}

