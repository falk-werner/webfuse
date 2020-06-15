#ifndef WF_LOOKUP_MATCHER_HPP
#define WF_LOOKUP_MATCHER_HPP

#include <gmock/gmock.h>
#include <jansson.h>
#include <cstring>

namespace webfuse_test
{

MATCHER_P2(Lookup, parent, name, "")
{
    if (!json_is_array(arg))
    { 
        *result_listener << "json array expected";
        return false;
    }
    
    json_t * parent_ = json_array_get(arg, 1);
    if (!json_is_integer(parent_))
    {
        *result_listener << "parent is expected to be an integer";
        return false;
    }
    if (parent != json_integer_value(parent_))
    {
        *result_listener << "parent mismatch: expected " << parent
            << " but was " << json_integer_value(parent_); 
        return false;
    }

    json_t * name_ = json_array_get(arg, 2);
    if (!json_is_string(name_))
    {
        *result_listener << "name is expected to be a string";
        return false;
    }
    if (0 != strcmp(name, json_string_value(name_)))
    {
        *result_listener << "name mismatch: expected \"" << name
            << "\" but was \"" << json_string_value(name_) << "\""; 
        return false;
    }

    return true;
}

}

#endif
