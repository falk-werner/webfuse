#ifndef MOCK_AUTHENTICATOR_H
#define MOCK_AUTHENTICATOR_H

#include <gmock/gmock.h>
#include "webfuse/adapter/impl/authenticator.h"

namespace webfuse_test
{

class Authenticator
{
public:
    virtual ~Authenticator() { }
    virtual bool authenticate(
        struct wf_credentials const * credentials,
        void * user_data) = 0;
};

class MockAuthenticator: public Authenticator
{
public:
    MOCK_METHOD2(authenticate, bool (struct wf_credentials const * credentials, void * user_data));
};

void set_authenticator(Authenticator * authenticator);
void set_authenticator(size_t index, Authenticator * authenticator);

bool authenticate(struct wf_credentials const * creds, void * user_data);
bool authenticate_1(struct wf_credentials const * creds, void * user_data);
bool authenticate_2(struct wf_credentials const * creds, void * user_data);

}

#endif
