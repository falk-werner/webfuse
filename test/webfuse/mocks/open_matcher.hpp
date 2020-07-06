#ifndef WF_OPEN_MATCHER_HPP
#define WF_OPEN_MATCHER_HPP

#include <gmock/gmock.h>
#include <jansson.h>

namespace webfuse_test
{

MATCHER_P(Open, inode, "")
{
    if (!json_is_array(arg))
    {
        *result_listener << "json array expected";
        return false;
    }

    json_t * inode_ = json_array_get(arg, 1);
    if (!json_is_integer(inode_))
    {
        *result_listener << "inode is expectoed to an integer";
        return false;
    }

    if (inode != json_integer_value(inode_))
    {
        *result_listener << "inode mismatch: expected" << inode 
            << " but was " << json_integer_value(inode_);
        return false;
    }

    return true;
}

}

#endif
