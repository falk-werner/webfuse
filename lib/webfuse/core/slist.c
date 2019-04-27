#include "webfuse/core/slist.h"
#include <stddef.h>

void wf_slist_init(
    struct wf_slist * list)
{
    list->head.next = NULL;
    list->last = &list->head;
}

bool wf_slist_empty(
    struct wf_slist * list)
{
    return (list->last == &list->head);
}

struct wf_slist_item * wf_slist_first(
    struct wf_slist * list)
{
    return list->head.next;
}

void wf_slist_append(
    struct wf_slist * list,
    struct wf_slist_item * item)
{
    item->next = NULL;
    list->last->next = item;
    list->last = item;
}

struct wf_slist_item * wf_slist_remove_first(
    struct wf_slist * list)
{
    return wf_slist_remove_after(list, &list->head);
}

struct wf_slist_item * wf_slist_remove_after(
    struct wf_slist * list,
    struct wf_slist_item * prev)
{

    struct wf_slist_item * result = prev->next;

    if (NULL != result)
    {
        prev->next = result->next;

        if (list->last == result)
        {
            list->last = prev;
        }    
    }

    return result;
}
