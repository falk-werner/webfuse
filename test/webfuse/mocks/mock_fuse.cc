#include "webfuse/mocks/mock_fuse.hpp"
#include "webfuse/utils/wrap.hpp"

extern "C"
{
static webfuse_test::FuseMock * webfuse_test_FuseMock = nullptr;

WF_WRAP_FUNC1(webfuse_test_FuseMock, void*, fuse_req_userdata, fuse_req_t);
WF_WRAP_FUNC2(webfuse_test_FuseMock, int, fuse_reply_open, fuse_req_t, const struct fuse_file_info *);
WF_WRAP_FUNC2(webfuse_test_FuseMock, int, fuse_reply_err, fuse_req_t, int);
WF_WRAP_FUNC3(webfuse_test_FuseMock, int,  fuse_reply_buf, fuse_req_t, const char *, size_t);
WF_WRAP_FUNC3(webfuse_test_FuseMock, int, fuse_reply_attr, fuse_req_t, const struct stat *, double);
WF_WRAP_FUNC1(webfuse_test_FuseMock, const struct fuse_ctx *, fuse_req_ctx, fuse_req_t);
WF_WRAP_FUNC2(webfuse_test_FuseMock, int, fuse_reply_entry, fuse_req_t, const struct fuse_entry_param *);
}

namespace webfuse_test
{

FuseMock::FuseMock()
{
    webfuse_test_FuseMock = this;
}

FuseMock::~FuseMock()
{
    webfuse_test_FuseMock = nullptr;
}


}