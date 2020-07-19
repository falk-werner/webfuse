#ifndef WF_LOOKUP_MATCHER_HPP
#define WF_LOOKUP_MATCHER_HPP

#include "webfuse/impl/json/node.h"
#include <gmock/gmock.h>
#include <cstring>

namespace webfuse_test
{

MATCHER_P2(Lookup, parent, name, "")
{
    if (!wf_impl_json_is_array(arg))
    { 
        *result_listener << "json array expected";
        return false;
    }
    
    wf_json const * parent_ = wf_impl_json_array_get(arg, 1);
    if (!wf_impl_json_is_int(parent_))
    {
        *result_listener << "parent is expected to be an integer";
        return false;
    }
    if (parent != wf_impl_json_int_get(parent_))
    {
        *result_listener << "parent mismatch: expected " << parent
            << " but was " << wf_impl_json_int_get(parent_); 
        return false;
    }

    wf_json const * name_ = wf_impl_json_array_get(arg, 2);
    if (!wf_impl_json_is_string(name_))
    {
        *result_listener << "name is expected to be a string";
        return false;
    }
    if (0 != strcmp(name, wf_impl_json_string_get(name_)))
    {
        *result_listener << "name mismatch: expected \"" << name
            << "\" but was \"" << wf_impl_json_string_get(name_) << "\""; 
        return false;
    }

    return true;
}

}

#endif
