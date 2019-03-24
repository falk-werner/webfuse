#include "mock_authenticator.hpp"

#define WSFS_AUTHENTICTOR_COUNT 3

namespace
{

wsfs_test::Authenticator * g_authenticators[WSFS_AUTHENTICTOR_COUNT];

}


namespace wsfs_test
{

void set_authenticator(Authenticator * authenticator)
{
    set_authenticator(0, authenticator);
}

void set_authenticator(size_t i, Authenticator * authenticator)
{
    g_authenticators[i] = authenticator;
}

bool authenticate(struct credentials * creds, void * user_data)
{
    return g_authenticators[0]->authenticate(creds, user_data);
}

bool authenticate_1(struct credentials * creds, void * user_data)
{
    return g_authenticators[1]->authenticate(creds, user_data);
}

bool authenticate_2(struct credentials * creds, void * user_data)
{
    return g_authenticators[2]->authenticate(creds, user_data);
}

    
}