#include <gtest/gtest.h>
#include "webfuse/adapter/client_tlsconfig.h"
#include "webfuse/adapter/impl/client_tlsconfig.h"

TEST(ClientTlsConfig, InitAndCleanup)
{
    wf_client_tlsconfig config;

    wf_impl_client_tlsconfig_init(&config);
    wf_impl_client_tlsconfig_cleanup(&config);
}

TEST(ClientTlsConfig, SetKeyPath)
{
    wf_client_tlsconfig config;
    wf_impl_client_tlsconfig_init(&config);

    wf_client_tlsconfig_set_keypath(&config, "/path/to/key.pem");
    ASSERT_STREQ("/path/to/key.pem", config.key_path);

    wf_impl_client_tlsconfig_cleanup(&config);
}

TEST(ClientTlsConfig, SetCertPath)
{
    wf_client_tlsconfig config;
    wf_impl_client_tlsconfig_init(&config);

    wf_client_tlsconfig_set_certpath(&config, "/path/to/cert.pem");
    ASSERT_STREQ("/path/to/cert.pem", config.cert_path);

    wf_impl_client_tlsconfig_cleanup(&config);
}

TEST(ClientTlsConfig, SetCafilePath)
{
    wf_client_tlsconfig config;
    wf_impl_client_tlsconfig_init(&config);

    wf_client_tlsconfig_set_cafilepath(&config, "/path/to/cafile.pem");
    ASSERT_STREQ("/path/to/cafile.pem", config.cafile_path);
    
    wf_impl_client_tlsconfig_cleanup(&config);
}

TEST(ClientTslConfig, IsSet)
{
    wf_client_tlsconfig config;

    wf_impl_client_tlsconfig_init(&config);
    ASSERT_FALSE(wf_impl_client_tlsconfig_isset(&config));
    wf_impl_client_tlsconfig_cleanup(&config);

    wf_impl_client_tlsconfig_init(&config);
    wf_client_tlsconfig_set_keypath(&config, "/path/to/key.pem");
    ASSERT_FALSE(wf_impl_client_tlsconfig_isset(&config));
    wf_impl_client_tlsconfig_cleanup(&config);

    wf_impl_client_tlsconfig_init(&config);
    wf_client_tlsconfig_set_certpath(&config, "/path/to/cert.pem");
    ASSERT_FALSE(wf_impl_client_tlsconfig_isset(&config));
    wf_impl_client_tlsconfig_cleanup(&config);

    wf_impl_client_tlsconfig_init(&config);
    wf_client_tlsconfig_set_keypath(&config, "/path/to/key.pem");
    wf_client_tlsconfig_set_certpath(&config, "/path/to/cert.pem");
    ASSERT_TRUE(wf_impl_client_tlsconfig_isset(&config));
    wf_impl_client_tlsconfig_cleanup(&config);
}