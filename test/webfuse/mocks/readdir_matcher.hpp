#ifndef WF_READDIR_MATCHER_HPP
#define WF_READDIR_MATCHER_HPP

#include <gmock/gmock.h>
#include "webfuse/impl/json/node.h"

namespace webfuse_test
{

MATCHER_P(ReadDir, inode, "")
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
