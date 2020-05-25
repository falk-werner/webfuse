#include "webfuse/mocks/mock_provider_client.hpp"
#include "webfuse/provider/operation/error.h"
#include "webfuse/provider/dirbuffer.h"

extern "C"
{
using webfuse_test::IProviderClient;
using webfuse_test::ProviderClientException;

static void webfuse_test_iproviderclient_onconnected(
    void * user_data)
{
    auto * self = reinterpret_cast<IProviderClient*>(user_data);
    self->OnConnected();
}

static void webfuse_test_iproviderclient_ondisconnected(
    void * user_data)
{
    auto * self = reinterpret_cast<IProviderClient*>(user_data);
    self->OnDisconnected();
}

static void webfuse_test_iproviderclient_onlookup(
    struct wfp_request * request,
    ino_t parent,
    char const * name,
    void * user_data)
{
    auto * self = reinterpret_cast<IProviderClient*>(user_data);

    try 
    {
        struct stat buffer;
        self->Lookup(parent, name, &buffer);
        wfp_respond_lookup(request, &buffer);
    }
    catch (ProviderClientException& ex)
    {
        wfp_respond_error(request, ex.GetErrorCode());
    }
    catch (...)
    {
        wfp_respond_error(request, WF_BAD);
    }
}

static void webfuse_test_iproviderclient_ongetattr(
    struct wfp_request * request,
    ino_t inode,
    void * user_data)
{
    auto * self = reinterpret_cast<IProviderClient*>(user_data);

    try 
    {
        struct stat buffer;
        memset(&buffer, 0, sizeof(struct stat));
        self->GetAttr(inode, &buffer);
        wfp_respond_getattr(request,&buffer);
    }
    catch (ProviderClientException& ex)
    {
        wfp_respond_error(request, ex.GetErrorCode());
    }
    catch (...)
    {
        wfp_respond_error(request, WF_BAD);
    }

}

static void webfuse_test_iproviderclient_onreaddir(
    struct wfp_request * request,
    ino_t directory,
    void * user_data)
{
    auto * self = reinterpret_cast<IProviderClient*>(user_data);
    wfp_dirbuffer * buffer = wfp_dirbuffer_create();

    try 
    {
        self->ReadDir(directory, buffer);
        wfp_respond_readdir(request, buffer);
    }
    catch (ProviderClientException& ex)
    {
        wfp_respond_error(request, ex.GetErrorCode());
    }
    catch (...)
    {
        wfp_respond_error(request, WF_BAD);
    }

    wfp_dirbuffer_dispose(buffer);

}

static void webfuse_test_iproviderclient_onopen(
    struct wfp_request * request,
    ino_t inode,
    int flags,
    void * user_data)
{
    auto * self = reinterpret_cast<IProviderClient*>(user_data);

    try 
    {
        uint32_t handle = 0;
        self->Open(inode, flags, &handle);
        wfp_respond_open(request, handle);
    }
    catch (ProviderClientException& ex)
    {
        wfp_respond_error(request, ex.GetErrorCode());
    }
    catch (...)
    {
        wfp_respond_error(request, WF_BAD);
    }

}

static void webfuse_test_iproviderclient_onclose(
    ino_t inode,
    uint32_t handle,
    int flags,
    void * user_data)
{
    auto * self = reinterpret_cast<IProviderClient*>(user_data);
    self->Close(inode, handle, flags);
}

static void webfuse_test_iproviderclient_onread(
        struct wfp_request * request,
    ino_t inode,
    uint32_t handle,
    size_t offset,
    size_t length,
    void * user_data)
{
    auto * self = reinterpret_cast<IProviderClient*>(user_data);
    char * data = new char[length];

    try 
    {
        size_t bytes_read = 0;
        self->Read(inode, handle, offset, length, data, &bytes_read);
        wfp_respond_read(request, data, bytes_read);
    }
    catch (ProviderClientException& ex)
    {
        wfp_respond_error(request, ex.GetErrorCode());
    }
    catch (...)
    {
        wfp_respond_error(request, WF_BAD);
    }

    delete[] data;
}

static void webfuse_test_iproviderclient_get_credentials(
    wfp_credentials * credentials,
    void * user_data)
{
    auto * self = reinterpret_cast<IProviderClient*>(user_data);

    try
    {
        self->GetCredentials(credentials);
    }
    catch (...)
    {
        // swallow
    }
}

}

namespace webfuse_test
{

ProviderClientException::ProviderClientException(wf_status error_code)
: runtime_error("ProviderClientException")
, error_code_(error_code)
{

}

wf_status ProviderClientException::GetErrorCode()
{
    return error_code_;
}


void IProviderClient::AttachTo(wfp_client_config * config, bool enableAuthentication)
{
    void * self = reinterpret_cast<void *>(this);
    wfp_client_config_set_userdata(config, self);
    wfp_client_config_set_onconnected(config, &webfuse_test_iproviderclient_onconnected);
    wfp_client_config_set_ondisconnected(config, &webfuse_test_iproviderclient_ondisconnected);

    wfp_client_config_set_onlookup(config, &webfuse_test_iproviderclient_onlookup);
    wfp_client_config_set_ongetattr(config, &webfuse_test_iproviderclient_ongetattr);
    wfp_client_config_set_onreaddir(config, &webfuse_test_iproviderclient_onreaddir);
    wfp_client_config_set_onopen(config, &webfuse_test_iproviderclient_onopen);
    wfp_client_config_set_onclose(config, &webfuse_test_iproviderclient_onclose);
    wfp_client_config_set_onread(config, &webfuse_test_iproviderclient_onread);

    if (enableAuthentication)
    {
        wfp_client_config_enable_authentication(config, &webfuse_test_iproviderclient_get_credentials);
    }
}

}