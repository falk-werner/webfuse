#ifndef WF_TEST_UTIL_MOUNTPOINT_FACTORY_HPP
#define WF_TEST_UTIL_MOUNTPOINT_FACTORY_HPP

extern "C"
{

struct wf_mountpoint;

extern wf_mountpoint *
webfuse_test_create_mountpoint(
    char const * filesystem,
    void * user_data);


}

#endif
