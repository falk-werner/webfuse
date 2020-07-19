#ifndef WF_OPEN_MATCHER_HPP
#define WF_OPEN_MATCHER_HPP

#include "webfuse/impl/json/node.h"
#include <gmock/gmock.h>

namespace webfuse_test
{

MATCHER_P(Open, inode, "")
{
    if (!wf_impl_json_is_array(arg))
    {
        *result_listener << "json array expected";
        return false;
    }

    wf_json const * inode_ = wf_impl_json_array_get(arg, 1);
    if (!wf_impl_json_is_int(inode_))
    {
        *result_listener << "inode is expectoed to an integer";
        return false;
    }

    if (inode != wf_impl_json_int_get(inode_))
    {
        *result_listener << "inode mismatch: expected" << inode 
            << " but was " << wf_impl_json_int_get(inode_);
        return false;
    }

    return true;
}

}

#endif
