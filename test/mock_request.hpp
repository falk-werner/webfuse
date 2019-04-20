#ifndef WF_MOCK_REQUEST_HPP
#define WF_MOCK_REQUEST_HPP

#include <gmock/gmock.h>
#include <jansson.h>
#include <cstring>
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

MATCHER_P3(GetAttrMatcher, inode, mode, file_type, "") 
{
    json_t * inode_holder = json_object_get(arg, "inode");
    if ((!json_is_integer(inode_holder)) || (inode != json_integer_value(inode_holder)))
    {
        return false;
    }

    json_t * mode_holder = json_object_get(arg, "mode");
    if ((!json_is_integer(mode_holder)) || (mode != json_integer_value(mode_holder)))
    {
        return false;
    }

    json_t * type_holder = json_object_get(arg, "type");
    if ((!json_is_string(type_holder)) || (0 != strcmp(file_type, json_string_value(type_holder))))
    {
        return false;
    }

    return true;
}


}


#endif
