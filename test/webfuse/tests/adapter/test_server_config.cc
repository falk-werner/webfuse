#include <gtest/gtest.h>
#include "webfuse/adapter/server_config.h"
#include "webfuse/adapter/impl/server_config.h"
#include "webfuse/adapter/impl/authenticator.h"
#include "webfuse/utils/tempdir.hpp"

using webfuse_test::TempDir;

namespace
{

wf_mountpoint * create_mountpoint(
    char const * filesystem,
    void * user_data)
{
    (void) filesystem;
    (void) user_data;

    return nullptr;
}

bool authenticate(
    wf_credentials const * credentials,
    void * user_data)
{
    (void) credentials;
    (void) user_data;

    return false;
}

}


TEST(server_config, create_dispose)
{
    wf_server_config * config = wf_server_config_create();
    ASSERT_NE(nullptr, config);

    wf_server_config_dispose(config);
}

TEST(server_config, set_documentroot)
{
    wf_server_config * config = wf_server_config_create();
    ASSERT_NE(nullptr, config);

    ASSERT_EQ(nullptr, config->document_root);

    wf_server_config_set_documentroot(config, "www");
    ASSERT_STREQ("www", config->document_root);
    
    wf_server_config_set_documentroot(config, "/var/www");
    ASSERT_STREQ("/var/www", config->document_root);

    wf_server_config_dispose(config);
}

TEST(server_config, set_keypath)
{
    wf_server_config * config = wf_server_config_create();
    ASSERT_NE(nullptr, config);

    ASSERT_EQ(nullptr, config->key_path);

    wf_server_config_set_keypath(config, "key.pem");
    ASSERT_STREQ("key.pem", config->key_path);
    
    wf_server_config_set_keypath(config, "pki/self/key.pem");
    ASSERT_STREQ("pki/self/key.pem", config->key_path);

    wf_server_config_dispose(config);
}

TEST(server_config, set_certpath)
{
    wf_server_config * config = wf_server_config_create();
    ASSERT_NE(nullptr, config);

    ASSERT_EQ(nullptr, config->key_path);

    wf_server_config_set_certpath(config, "cert.pem");
    ASSERT_STREQ("cert.pem", config->cert_path);
    
    wf_server_config_set_certpath(config, "pki/self/cert.pem");
    ASSERT_STREQ("pki/self/cert.pem", config->cert_path);

    wf_server_config_dispose(config);
}

TEST(server_config, set_vhostname)
{
    wf_server_config * config = wf_server_config_create();
    ASSERT_NE(nullptr, config);

    ASSERT_EQ(nullptr, config->key_path);

    wf_server_config_set_vhostname(config, "webfuse");
    ASSERT_STREQ("webfuse", config->vhost_name);
    
    wf_server_config_set_vhostname(config, "localhost");
    ASSERT_STREQ("localhost", config->vhost_name);

    wf_server_config_dispose(config);
}

TEST(server_config, set_port)
{
    wf_server_config * config = wf_server_config_create();
    ASSERT_NE(nullptr, config);

    ASSERT_EQ(0, config->port);

    wf_server_config_set_port(config, 8443);
    ASSERT_EQ(8443, config->port);
    
    wf_server_config_set_port(config, 8080);
    ASSERT_EQ(8080, config->port);

    wf_server_config_dispose(config);
}

TEST(server_config, set_mounpoint_factory)
{
    wf_server_config * config = wf_server_config_create();
    ASSERT_NE(nullptr, config);
    ASSERT_EQ(nullptr, config->mountpoint_factory.create_mountpoint);
    ASSERT_EQ(nullptr, config->mountpoint_factory.user_data);

    int value = 42;
    void * user_data = reinterpret_cast<void*>(&value);
    wf_server_config_set_mountpoint_factory(config, &create_mountpoint, user_data);
    ASSERT_EQ(&create_mountpoint, config->mountpoint_factory.create_mountpoint);
    ASSERT_EQ(user_data, config->mountpoint_factory.user_data);

    wf_server_config_dispose(config);
}

TEST(server_config, add_authenticator)
{
    wf_server_config * config = wf_server_config_create();
    ASSERT_NE(nullptr, config);
    ASSERT_EQ(nullptr, config->authenticators.first);

    int value = 42;
    void * user_data = reinterpret_cast<void*>(&value);
    wf_server_config_add_authenticator(config, "username", &authenticate, user_data);

    wf_impl_authenticator * authenticator = config->authenticators.first;
    ASSERT_STREQ("username", authenticator->type);
    ASSERT_EQ(&authenticate, authenticator->authenticate);
    ASSERT_EQ(user_data, authenticator->user_data);

    wf_server_config_dispose(config);
}