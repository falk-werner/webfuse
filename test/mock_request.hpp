#ifndef WF_MOCK_REQUEST_HPP
#define WF_MOCK_REQUEST_HPP

#include <gmock/gmock.h>
#include <jansson.h>
#include "webfuse/provider/impl/request.h"


namespace webfuse_test
{

class Request
{
public:
    virtual ~Request() { }
    virtual void respond(json_t * result, int id) = 0;
    virtual void respond_error(json_t * error, int id) = 0;
};

class MockRequest: public Request
{
public:
    MOCK_METHOD2(respond, void(json_t * result, int id));
    MOCK_METHOD2(respond_error, void(json_t * error, int id));
};

extern struct wfp_request *
request_create(
    Request * req,
    int id);

}


#endif
