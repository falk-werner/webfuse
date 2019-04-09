#ifndef WF_DLIST_H
#define WF_DLIST_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_dlist_item
{
    struct wf_dlist_item * prev;
    struct wf_dlist_item * next;
};

struct wf_dlist
{
    struct wf_dlist_item * first;
};


typedef void wf_dlist_item_cleanup_fn(
    struct wf_dlist_item * item,
    void * user_data);

typedef bool wf_dlist_item_predicate_fn(
    struct wf_dlist_item * item,
    void * user_data);

extern void wf_dlist_item_init(
    struct wf_dlist_item * item);

extern void wf_dlist_item_cleanup(
    struct wf_dlist_item * item);

extern void wf_dlist_init(
    struct wf_dlist * list);

extern void wf_dlist_cleanup(
    struct wf_dlist * list,
    wf_dlist_item_cleanup_fn * cleanup,
    void * user_data);

extern void wf_dlist_prepend(
    struct wf_dlist * list,
    struct wf_dlist_item * item);

extern void wf_dlist_remove(
    struct wf_dlist * list,
    struct wf_dlist_item * item);

extern struct wf_dlist_item * wf_dlist_find_first(
    struct wf_dlist * list,
    wf_dlist_item_predicate_fn * predicate,
    void * user_data);


#ifdef __cplusplus
}
#endif

#endif
