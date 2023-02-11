#include "webfuse/response_type.hpp"
#include <gtest/gtest.h>

using webfuse::request_type;

class response_type_test: public testing::TestWithParam<request_type> { };

TEST_P(response_type_test, conversion)
{
    auto const value = GetParam();
    auto const actual = webfuse::get_response_type(value);
    auto const expected = static_cast<webfuse::response_type>(static_cast<uint8_t>(value) | 0x80);
    
    ASSERT_EQ(expected, actual);
}

INSTANTIATE_TEST_CASE_P(response_type_values, response_type_test,
    testing::Values(
        request_type::access, request_type::getattr, request_type::readlink,
        request_type::symlink, request_type::link, request_type::link,
        request_type::rename, request_type::chmod, request_type::chown,
        request_type::truncate, request_type::fsync, request_type::open,
        request_type::mknod, request_type::create, request_type::release,
        request_type::unlink, request_type::read, request_type::write,
        request_type::mkdir, request_type::readdir, request_type::rmdir,
        request_type::statfs, request_type::utimens, request_type::unknown)
);

TEST(respones_type, unknown_values)
{
    auto const expected = webfuse::response_type::unknown;

    ASSERT_EQ(expected, webfuse::get_response_type(static_cast<request_type>(0x20)));
    ASSERT_EQ(expected, webfuse::get_response_type(static_cast<request_type>(0x30)));
    ASSERT_EQ(expected, webfuse::get_response_type(static_cast<request_type>(80)));
    ASSERT_EQ(expected, webfuse::get_response_type(static_cast<request_type>(0x42)));
    ASSERT_EQ(expected, webfuse::get_response_type(static_cast<request_type>(0xff)));
}
