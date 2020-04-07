#include "webfuse/mocks/mock_provider.hpp"

extern "C"
{
using webfuse_test::MockProvider;

static void webfuse_test_MockProvider_connected(
    void * user_data)
{
    auto * provider = reinterpret_cast<MockProvider*>(user_data);
    provider->connected();
}

static void webfuse_test_MockProvider_disconnected(
    void * user_data)
{
    auto * provider = reinterpret_cast<MockProvider*>(user_data);
    provider->disconnected();
}

static void webfuse_test_MockProvider_lookup(
    wfp_request * request,
    ino_t parent,
    char const * name,
    void * user_data)
{
    auto * provider = reinterpret_cast<MockProvider*>(user_data);
    provider->lookup(request, parent, name);
}

static void webfuse_test_MockProvider_getattr(
    wfp_request * request,
    ino_t inode,
    void * user_data)
{
    auto * provider = reinterpret_cast<MockProvider*>(user_data);
    provider->getattr(request, inode);
}

static void webfuse_test_MockProvider_readdir(
    wfp_request * request,
    ino_t directory,
    void * user_data)
{
    auto * provider = reinterpret_cast<MockProvider*>(user_data);
    provider->readdir(request, directory);
}

static void webfuse_test_MockProvider_open(
    wfp_request * request,
    ino_t inode,
    int flags,
    void * user_data)
{
    auto * provider = reinterpret_cast<MockProvider*>(user_data);
    provider->open(request, inode, flags);

}

static void webfuse_test_MockProvider_close(
    ino_t inode,
    uint32_t handle,
    int flags,
    void * user_data)
{
    auto * provider = reinterpret_cast<MockProvider*>(user_data);
    provider->close(inode, handle, flags);

}

static void webfuse_test_MockProvider_read(
    wfp_request * request,
    ino_t inode,
    uint32_t handle,
    size_t offset,
    size_t length,
    void * user_data)
{
    auto * provider = reinterpret_cast<MockProvider*>(user_data);
    provider->read(request, inode, handle, offset, length);
}

static void webfuse_test_MockProvider_get_credentials(
    wfp_credentials * credentials,
    void * user_data)
{
    auto * provider = reinterpret_cast<MockProvider*>(user_data);
    provider->get_credentials(credentials);
}


static wfp_provider const webfuse_test_MockProvider =
{
    &webfuse_test_MockProvider_connected,
    &webfuse_test_MockProvider_disconnected,
    &webfuse_test_MockProvider_lookup,
    &webfuse_test_MockProvider_getattr,
    &webfuse_test_MockProvider_readdir,
    &webfuse_test_MockProvider_open,
    &webfuse_test_MockProvider_close,
    &webfuse_test_MockProvider_read,
    &webfuse_test_MockProvider_get_credentials,
};

}

namespace webfuse_test
{

MockProvider::MockProvider()
{

}

MockProvider::~MockProvider()
{

}

wfp_provider const * MockProvider::get_provider()
{
    return &webfuse_test_MockProvider;
}

void * MockProvider::get_userdata()
{
    return reinterpret_cast<void*>(this);
}

}