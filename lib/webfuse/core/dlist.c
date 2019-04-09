#include "webfuse/core/dlist.h"
#include "webfuse/core/util.h"
#include <stddef.h>

void wf_dlist_item_init(
    struct wf_dlist_item * item)
{
    item->next = NULL;
    item->prev = NULL;
}

void wf_dlist_item_cleanup(
    struct wf_dlist_item * item)
{
    item->next = NULL;
    item->prev = NULL;
}

void wf_dlist_init(
    struct wf_dlist * list)
{
    list->first = NULL;
}

static void wf_dlist_item_cleanup_default(
    struct wf_dlist_item * WF_UNUSED_PARAM(item),
    void * WF_UNUSED_PARAM(user_data))
{
    // empty
}

void wf_dlist_cleanup(
    struct wf_dlist * list,
    wf_dlist_item_cleanup_fn * cleanup,
    void * user_data)
{
    wf_dlist_item_cleanup_fn * effective_cleanup = (NULL != cleanup) ? cleanup : &wf_dlist_item_cleanup_default;
    struct wf_dlist_item * item = list->first;
 
    while (NULL != item)
    {
        struct wf_dlist_item * next = item->next;
        effective_cleanup(item, user_data);
        item = next;
    }

    list->first = NULL;
}

void wf_dlist_prepend(
    struct wf_dlist * list,
    struct wf_dlist_item * item)
{
    if (NULL != list->first)
    {
        list->first->prev = item;
    }

    item->prev = NULL;
    item->next = list->first;
    list->first = item;
}

void wf_dlist_remove(
    struct wf_dlist * list,
    struct wf_dlist_item * item)
{
    if (NULL != item->prev)
    {
        item->prev->next = item->next;
    }

    if (NULL != item->next)
    {
        item->next->prev = item->prev;
    }

    if (list->first == item)
    {
        list->first = item->next;
    }
}

struct wf_dlist_item * wf_dlist_find_first(
    struct wf_dlist * list,
    wf_dlist_item_predicate_fn * predicate,
    void * user_data)
{
    struct wf_dlist_item * item = list->first;

    while (NULL != item)
    {
        if (predicate(item, user_data))
        {
            break;
        }
        item = item->next;
    }

    return item;
}
