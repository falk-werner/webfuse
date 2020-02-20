#include <gtest/gtest.h>
#include "webfuse/core/base64.h"

TEST(Base64, EncodedSize)
{
    ASSERT_EQ(4, wf_base64_encoded_size(1));
    ASSERT_EQ(4, wf_base64_encoded_size(2));
    ASSERT_EQ(4, wf_base64_encoded_size(3));

    ASSERT_EQ(8, wf_base64_encoded_size(4));
    ASSERT_EQ(8, wf_base64_encoded_size(5));
    ASSERT_EQ(8, wf_base64_encoded_size(6));

    ASSERT_EQ(120, wf_base64_encoded_size(90));
}

TEST(Base64, Encode)
{
    char buffer[42];

    std::string in = "Hello";
    size_t length = wf_base64_encode((uint8_t const*) in.c_str(), in.size(), buffer, 42);
    ASSERT_EQ(8, length);
    ASSERT_STREQ("SGVsbG8=", buffer);

    in = "Hello\n";
    length = wf_base64_encode((uint8_t const*) in.c_str(), in.size(), buffer, 42);
    ASSERT_EQ(8, length);
    ASSERT_STREQ("SGVsbG8K", buffer);

    in = "Blue";
    length = wf_base64_encode((uint8_t const*) in.c_str(), in.size(), buffer, 42);
    ASSERT_EQ(8, length);
    ASSERT_STREQ("Qmx1ZQ==", buffer);
}

TEST(Base64, FailedToEncodeBufferTooSmall)
{
    char buffer[1];

    std::string in = "Hello";
    size_t length = wf_base64_encode((uint8_t const*) in.c_str(), in.size(), buffer, 1);
    ASSERT_EQ(0, length);
}

TEST(Base64, DecodedSize)
{
    std::string in = "SGVsbG8=";    // Hello
    size_t length = wf_base64_decoded_size(in.c_str(), in.size());
    ASSERT_EQ(5, length);

    in = "SGVsbG8K";    // Hello\n
    length = wf_base64_decoded_size(in.c_str(), in.size());
    ASSERT_EQ(6, length);

    in = "Qmx1ZQ==";    // Blue
    length = wf_base64_decoded_size(in.c_str(), in.size());
    ASSERT_EQ(4, length);
}

TEST(Base64, IsValid)
{
    std::string in = "SGVsbG8=";    // Hello
    ASSERT_TRUE(wf_base64_isvalid(in.c_str(), in.size()));

    in = "SGVsbG8K";    // Hello\n
    ASSERT_TRUE(wf_base64_isvalid(in.c_str(), in.size()));

    in = "Qmx1ZQ==";    // Blue
    ASSERT_TRUE(wf_base64_isvalid(in.c_str(), in.size()));

    in = "Qmx1ZQ=a";
    ASSERT_FALSE(wf_base64_isvalid(in.c_str(), in.size()));

    in = "Qmx1ZQ";
    ASSERT_FALSE(wf_base64_isvalid(in.c_str(), in.size()));

    in = "Qmx1ZQ=";
    ASSERT_FALSE(wf_base64_isvalid(in.c_str(), in.size()));

    in = "Qmx1Z===";
    ASSERT_FALSE(wf_base64_isvalid(in.c_str(), in.size()));

    in = "Qmx1ZQ?=";
    ASSERT_FALSE(wf_base64_isvalid(in.c_str(), in.size()));

    in = "Qm?1ZQ==";
    ASSERT_FALSE(wf_base64_isvalid(in.c_str(), in.size()));
}

TEST(Base64, Decode)
{
    char buffer[42];

    std::string in = "SGVsbG8=";    // Hello
    size_t length = wf_base64_decode(in.c_str(), in.size(), (uint8_t*) buffer, 42);
    ASSERT_EQ(5, length);
    buffer[length] = '\0';
    ASSERT_STREQ("Hello", buffer);

    in = "SGVsbG8K";    // Hello\n
    length = wf_base64_decode(in.c_str(), in.size(), (uint8_t*) buffer, 42);
    ASSERT_EQ(6, length);
    buffer[length] = '\0';
    ASSERT_STREQ("Hello\n", buffer);

    in = "Qmx1ZQ==";    // Blue
    length = wf_base64_decode(in.c_str(), in.size(), (uint8_t*) buffer, 42);
    ASSERT_EQ(4, length);
    buffer[length] = '\0';
    ASSERT_STREQ("Blue", buffer);
}

TEST(Base64, FailToDecodeBufferTooSmall)
{
    char buffer[1];

    std::string in = "SGVsbG8=";    // Hello
    size_t length = wf_base64_decode(in.c_str(), in.size(), (uint8_t*) buffer, 1);
    ASSERT_EQ(0, length);
}

