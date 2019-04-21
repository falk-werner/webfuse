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
        *result_listener << "missing inode";
        return false;
    }

    json_t * mode_holder = json_object_get(arg, "mode");
    if ((!json_is_integer(mode_holder)) || (mode != json_integer_value(mode_holder)))
    {
        *result_listener << "missing mode";
        return false;
    }

    json_t * type_holder = json_object_get(arg, "type");
    if ((!json_is_string(type_holder)) || (0 != strcmp(file_type, json_string_value(type_holder))))
    {
        *result_listener << "missing type";
        return false;
    }

    return true;
}


MATCHER_P(ReaddirMatcher, contained_elements , "") 
{
    if (!json_is_array(arg))
    {
        *result_listener << "result is not array";
        return false;
    }

    {
        size_t i;
        json_t * value;

        json_array_foreach(arg, i, value)
        {
            json_t * inode = json_object_get(value, "inode");
            json_t * name = json_object_get(value, "name");

            if(!json_is_integer(inode))
            {
                *result_listener << "invalid result: missing inode";
                return false;
            }

            if (!json_is_string(name))
            {
                *result_listener << "invalid result: missing name";
                return false;
            }
        }
    }

    for(size_t i = 0; NULL != contained_elements[i]; i++)
    {
        char const * element = contained_elements[i];
        bool found = false;
        size_t j;
        json_t * value;

        json_array_foreach(arg, j, value)
        {
            json_t * name = json_object_get(value, "name");

            found  = (0 == strcmp(element, json_string_value(name)));
            if (found)
            {
                break;
            }
        }

        if (!found)
        {
            *result_listener << "missing required directory element: " << element;
            return false;
        }
    }

    return true;
}

}


#endif
