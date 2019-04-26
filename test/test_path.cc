#include <gtest/gtest.h>
#include "webfuse/core/path.h"

TEST(wf_path, empty)
{
    struct wf_path * path = wf_path_create("");
    ASSERT_EQ(0, wf_path_element_count(path));
    ASSERT_EQ(nullptr, wf_path_get_element(path, 0));

    wf_path_dispose(path);
}

TEST(wf_path, relative_file)
{
    struct wf_path * path = wf_path_create("some.file");
    ASSERT_EQ(1, wf_path_element_count(path));
    ASSERT_STREQ("some.file", wf_path_get_element(path, 0));

    wf_path_dispose(path);
}

TEST(wf_path, absolute_file)
{
    struct wf_path * path = wf_path_create("/absolute.file");
    ASSERT_EQ(1, wf_path_element_count(path));
    ASSERT_STREQ("absolute.file", wf_path_get_element(path, 0));

    wf_path_dispose(path);
}

TEST(wf_path, nested_path)
{
    struct wf_path * path = wf_path_create("/a/nested/path");
    ASSERT_EQ(3, wf_path_element_count(path));
    ASSERT_STREQ("a", wf_path_get_element(path, 0));
    ASSERT_STREQ("nested", wf_path_get_element(path, 1));
    ASSERT_STREQ("path", wf_path_get_element(path, 2));

    wf_path_dispose(path);
}

TEST(wf_path, deep_nested_path)
{
    struct wf_path * path = wf_path_create("/this/is/a/very/deep/nested/path/to/some/file");
    ASSERT_EQ(10, wf_path_element_count(path));
    ASSERT_STREQ("this", wf_path_get_element(path, 0));
    ASSERT_STREQ("is", wf_path_get_element(path, 1));
    ASSERT_STREQ("a", wf_path_get_element(path, 2));
    ASSERT_STREQ("very", wf_path_get_element(path, 3));
    ASSERT_STREQ("deep", wf_path_get_element(path, 4));
    ASSERT_STREQ("nested", wf_path_get_element(path, 5));
    ASSERT_STREQ("path", wf_path_get_element(path, 6));
    ASSERT_STREQ("to", wf_path_get_element(path, 7));
    ASSERT_STREQ("some", wf_path_get_element(path, 8));
    ASSERT_STREQ("file", wf_path_get_element(path, 9));

    wf_path_dispose(path);
}