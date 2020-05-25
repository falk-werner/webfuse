#ifndef WF_MOCK_PROVIDER_CLIENT_HPP
#define WF_MOCK_PROVIDER_CLIENT_HPP

#include <gmock/gmock.h>
#include "webfuse/provider/client_config.h"
#include "webfuse/core/status.h"
#include <stdexcept>

namespace webfuse_test
{
    class ProviderClientException: public std::runtime_error
    {
        public:
            explicit ProviderClientException(wf_status error_code);
            wf_status GetErrorCode();
        private:
            wf_status error_code_;            
    };

    class IProviderClient
    {
        public:
            virtual ~IProviderClient() = default;
            virtual void OnConnected() = 0;
            virtual void OnDisconnected() = 0;
            virtual void Lookup(ino_t parent, char const * name, struct stat * result) = 0;
            virtual void GetAttr(ino_t inode, struct stat * buffer) = 0;
            virtual void ReadDir(ino_t directory, wfp_dirbuffer * buffer) = 0;
            virtual void Open(ino_t inode, int flags, uint32_t * handle) = 0;
            virtual void Close(ino_t inode, uint32_t handle, int flags) = 0;
            virtual void Read(ino_t inode, uint32_t handle, size_t offset, size_t length, char * buffer, size_t * bytes_read) = 0;
            virtual void GetCredentials(wfp_credentials * credentials) = 0;

            void AttachTo(wfp_client_config * config, bool enableAuthentication = false);
    };

    class MockProviderClient: public IProviderClient
    {
        public:
            ~MockProviderClient() override = default;
            MOCK_METHOD0( OnConnected, void());
            MOCK_METHOD0( OnDisconnected, void());
            MOCK_METHOD3( Lookup, void(ino_t parent, char const * name, struct stat * result));
            MOCK_METHOD2( GetAttr, void(ino_t inode, struct stat * buffer));
            MOCK_METHOD2( ReadDir, void(ino_t directory, wfp_dirbuffer * buffer));
            MOCK_METHOD3( Open, void(ino_t inode, int flags, uint32_t * handle));
            MOCK_METHOD3( Close, void(ino_t inode, uint32_t handle, int flags));
            MOCK_METHOD6( Read, void(ino_t inode, uint32_t handle, size_t offset, size_t length, char * buffer, size_t * bytes_read));
            MOCK_METHOD1( GetCredentials, void (wfp_credentials * credentials));
    };
}

#endif
