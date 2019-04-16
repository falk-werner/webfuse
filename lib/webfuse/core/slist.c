#include "webfuse/core/slist.h"
#include <stddef.h>

void wf_slist_init(
    struct wf_slist * list)
{
    list->first = NULL;
    list->last = NULL;
}

bool wf_slist_empty(
    struct wf_slist * list)
{
    return (NULL == list->first);
}

void wf_slist_append(
    struct wf_slist * list,
    struct wf_slist_item * item)
{
    item->next = NULL;

    if (NULL != list->last)
    {
        list->last->next = item;
        list->last = item;
    }
    else
    {
        list->first = item;
        list->last = item;
    }
}

struct wf_slist_item * wf_slist_remove_first(
    struct wf_slist * list)
{
    struct wf_slist_item * const result = list->first;
    if (NULL != result)
    {
        list->first = list->first->next;
        if (NULL == list->first)
        {
            list->last = NULL;
        }
    } 

    return result;
}

struct wf_slist_item * wf_slist_remove_after(
    struct wf_slist * list,
    struct wf_slist_item * prev)
{
    struct wf_slist_item * result = NULL;

    if (NULL != prev)
    {
        result = prev->next;
        if ((NULL != result) && (NULL != result->next))
        {
            prev->next = result->next;
        }
        else
        {
            list->last = prev;
            prev->next = NULL;
        }
        
    }
    else
    {
        result = wf_slist_remove_first(list);
    }

    return result;
}
