#ifndef MOCK_AUTHENTICATOR_H
#define MOCK_AUTHENTICATOR_H

#include <gmock/gmock.h>
#include "wsfs/adapter/impl/authenticator.h"

namespace wsfs_test
{

class Authenticator
{
public:
    virtual ~Authenticator() { }
    virtual bool authenticate(
        struct credentials * credentials,
        void * user_data) = 0;
};

class MockAuthenticator: public Authenticator
{
public:
    MOCK_METHOD2(authenticate, bool (struct credentials * credentials, void * user_data));
};

void set_authenticator(Authenticator * authenticator);
void set_authenticator(size_t index, Authenticator * authenticator);

bool authenticate(struct credentials * creds, void * user_data);
bool authenticate_1(struct credentials * creds, void * user_data);
bool authenticate_2(struct credentials * creds, void * user_data);

}

#endif
