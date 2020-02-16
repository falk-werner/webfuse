#include <gtest/gtest.h>
#include "webfuse_adapter.h"
#include "webfuse/adapter/impl/uuid_mountpoint_factory.h"
#include "tempdir.hpp"
#include "file_utils.hpp"

using webfuse_test::TempDir;
using webfuse_test::is_dir;

TEST(uuid_mountpoint_factory, create_existing_dir)
{
    TempDir temp("uuid_mountpoint_factory");

    struct wf_impl_mountpoint_factory factory;
    bool factory_created = wf_impl_uuid_mountpoint_factory_init(&factory, temp.path());
    ASSERT_TRUE(factory_created);
    ASSERT_TRUE(is_dir(temp.path()));

    wf_mountpoint * mountpoint = wf_impl_mountpoint_factory_create_mountpoint(&factory, "dummy");
    std::string path = wf_mountpoint_get_path(mountpoint);
    ASSERT_TRUE(is_dir(path));

    wf_mountpoint_dispose(mountpoint);
    ASSERT_FALSE(is_dir(path));

    wf_impl_mountpoint_factory_cleanup(&factory);
    // keep dir not created by factory
    ASSERT_TRUE(is_dir(temp.path()));
}

TEST(uuid_mountpoint_factory, create_nonexisting_dir)
{
    TempDir temp("uuid_mountpoint_factory");
    std::string root_path = std::string(temp.path()) + "/root";

    struct wf_impl_mountpoint_factory factory;
    bool factory_created = wf_impl_uuid_mountpoint_factory_init(&factory, root_path.c_str());
    ASSERT_TRUE(factory_created);
    ASSERT_TRUE(is_dir(root_path));

    wf_mountpoint * mountpoint = wf_impl_mountpoint_factory_create_mountpoint(&factory, "dummy");
    std::string path = wf_mountpoint_get_path(mountpoint);
    ASSERT_TRUE(is_dir(path));

    wf_mountpoint_dispose(mountpoint);
    ASSERT_FALSE(is_dir(path));

    wf_impl_mountpoint_factory_cleanup(&factory);
    // remove dir, created by factory
    ASSERT_FALSE(is_dir(root_path));
}

TEST(uuid_mountpoint_factory, fail_to_created_nested_dir)
{
    TempDir temp("uuid_mountpoint_factory");
    std::string root_path = std::string(temp.path()) + "/nested/root";

    struct wf_impl_mountpoint_factory factory;
    bool factory_created = wf_impl_uuid_mountpoint_factory_init(&factory, root_path.c_str());
    ASSERT_FALSE(factory_created);
}