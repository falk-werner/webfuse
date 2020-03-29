#ifndef WF_TIMER_ON_TIMER_FN_H
#define WF_TIMER_ON_TIMER_FN_H

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_timer;

typedef void wf_timer_on_timer_fn(
    struct wf_timer * timer,
    void * user_data);

#ifdef __cplusplus
}
#endif

#endif
