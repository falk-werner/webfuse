#include "webfuse/mocks/mock_authenticator.hpp"

#define WF_AUTHENTICATOR_COUNT 3

namespace
{

webfuse_test::Authenticator * g_authenticators[WF_AUTHENTICATOR_COUNT];

}


namespace webfuse_test
{

void set_authenticator(Authenticator * authenticator)
{
    set_authenticator(0, authenticator);
}

void set_authenticator(size_t i, Authenticator * authenticator)
{
    g_authenticators[i] = authenticator;
}

bool authenticate(struct wf_credentials const * creds, void * user_data)
{
    return g_authenticators[0]->authenticate(creds, user_data);
}

bool authenticate_1(struct wf_credentials const * creds, void * user_data)
{
    return g_authenticators[1]->authenticate(creds, user_data);
}

bool authenticate_2(struct wf_credentials const * creds, void * user_data)
{
    return g_authenticators[2]->authenticate(creds, user_data);
}

    
}