#include <gtest/gtest.h>
#include "webfuse/core/slist.h"

TEST(wf_slist_remove_after, RemoveFirst)
{
    struct wf_slist list;
    struct wf_slist_item item[10];

    wf_slist_init(&list);
    wf_slist_append(&list, &item[0]);

    wf_slist_item * removed = wf_slist_remove_after(&list, NULL);
    ASSERT_TRUE(wf_slist_empty(&list));
    ASSERT_EQ(nullptr, list.first);
    ASSERT_EQ(nullptr, list.last);
    ASSERT_EQ(&item[0], removed);
}

TEST(wf_slist_remove_after, RemoveLast)
{
    struct wf_slist list;
    struct wf_slist_item item[10];

    wf_slist_init(&list);
    wf_slist_append(&list, &item[0]);
    wf_slist_append(&list, &item[1]);
    wf_slist_append(&list, &item[2]);

    wf_slist_item * removed = wf_slist_remove_after(&list, &item[1]);
    ASSERT_FALSE(wf_slist_empty(&list));
    ASSERT_EQ(&item[0], list.first);
    ASSERT_EQ(&item[1], list.last);
    ASSERT_EQ(nullptr, item[1].next);
    ASSERT_EQ(&item[2], removed);
}
