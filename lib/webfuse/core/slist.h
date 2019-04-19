#ifndef WF_SLIST_H
#define WF_SLIST_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C"
{ 
#endif

struct wf_slist_item
{
    struct wf_slist_item * next;
};

struct wf_slist
{
    struct wf_slist_item head;
    struct wf_slist_item * last;
};

extern void wf_slist_init(
    struct wf_slist * list);

extern bool wf_slist_empty(
    struct wf_slist * list);

extern struct wf_slist_item * wf_slist_first(
    struct wf_slist * list);

extern void wf_slist_append(
    struct wf_slist * list,
    struct wf_slist_item * item);

extern struct wf_slist_item * wf_slist_remove_first(
    struct wf_slist * list);

extern struct wf_slist_item * wf_slist_remove_after(
    struct wf_slist * list,
    struct wf_slist_item * prev);

#ifdef __cplusplus
}
#endif

#endif
