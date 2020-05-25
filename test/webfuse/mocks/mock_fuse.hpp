#ifndef MOCK_FUSE_HPP
#define MOCK_FUSE_HPP

#include "webfuse/adapter/impl/fuse_wrapper.h"
#include <gmock/gmock.h>

namespace webfuse_test
{

class FuseMock
{
public:
    FuseMock();
    virtual ~FuseMock();

    MOCK_METHOD1(fuse_req_userdata, void *(fuse_req_t req));
    MOCK_METHOD2(fuse_reply_open, int (fuse_req_t req, const struct fuse_file_info *fi));
    MOCK_METHOD2(fuse_reply_err, int (fuse_req_t req, int err));
    MOCK_METHOD3(fuse_reply_buf, int (fuse_req_t req, const char *buf, size_t size));
    MOCK_METHOD3(fuse_reply_attr, int (fuse_req_t req, const struct stat *attr, double attr_timeout));
    MOCK_METHOD1(fuse_req_ctx, const struct fuse_ctx *(fuse_req_t req));
    MOCK_METHOD2(fuse_reply_entry, int (fuse_req_t req, const struct fuse_entry_param *e));
};

}

#endif
