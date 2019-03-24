#include <gtest/gtest.h>

#include "wsfs/provider/impl/url.h"

TEST(url, ParseWs)
{
    struct wsfsp_impl_url url;
    bool result = wsfsp_impl_url_init(&url, "ws://localhost/");
    ASSERT_TRUE(result);
    ASSERT_EQ(80, url.port);
    ASSERT_FALSE(url.use_tls);
    ASSERT_STREQ("localhost", url.host);
    ASSERT_STREQ("/", url.path);

    wsfsp_impl_url_cleanup(&url);
}

TEST(url, ParswWss)
{
    struct wsfsp_impl_url url;
    bool result = wsfsp_impl_url_init(&url, "wss://localhost/");
    ASSERT_TRUE(result);
    ASSERT_EQ(443, url.port);
    ASSERT_TRUE(url.use_tls);
    ASSERT_STREQ("localhost", url.host);
    ASSERT_STREQ("/", url.path);

    wsfsp_impl_url_cleanup(&url);
}

TEST(url, ParseIPAdress)
{
    struct wsfsp_impl_url url;
    bool result = wsfsp_impl_url_init(&url, "ws://127.0.0.1/");
    ASSERT_TRUE(result);
    ASSERT_EQ(80, url.port);
    ASSERT_STREQ("127.0.0.1", url.host);
    ASSERT_STREQ("/", url.path);

    wsfsp_impl_url_cleanup(&url);
}

TEST(url, ParsePort)
{
    struct wsfsp_impl_url url;
    bool result = wsfsp_impl_url_init(&url, "ws://localhost:54321/");
    ASSERT_TRUE(result);
    ASSERT_EQ(54321, url.port);

    wsfsp_impl_url_cleanup(&url);
}

TEST(url, ParseNonEmptyPath)
{
    struct wsfsp_impl_url url;
    bool result = wsfsp_impl_url_init(&url, "ws://localhost/some_path?query");
    ASSERT_TRUE(result);
    ASSERT_STREQ("/some_path?query", url.path);

    wsfsp_impl_url_cleanup(&url);
}

TEST(url, FailToParseUnknownProtocol)
{
    struct wsfsp_impl_url url;
    bool result = wsfsp_impl_url_init(&url, "unknown://localhost/");
    ASSERT_FALSE(result);
    ASSERT_EQ(0, url.port);
    ASSERT_EQ(nullptr, url.path);
    ASSERT_EQ(nullptr, url.host);
}

TEST(url, FailToParseMissingProtocol)
{
    struct wsfsp_impl_url url;
    bool result = wsfsp_impl_url_init(&url, "unknown");
    ASSERT_FALSE(result);
    ASSERT_EQ(0, url.port);
    ASSERT_EQ(nullptr, url.path);
    ASSERT_EQ(nullptr, url.host);
}

TEST(url, FailToParseMissingPath)
{
    struct wsfsp_impl_url url;
    bool result = wsfsp_impl_url_init(&url, "ws://localhost");
    ASSERT_FALSE(result);
    ASSERT_EQ(0, url.port);
    ASSERT_EQ(nullptr, url.path);
    ASSERT_EQ(nullptr, url.host);
}

