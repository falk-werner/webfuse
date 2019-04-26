#include <gtest/gtest.h>
#include "webfuse/core/slist.h"

TEST(wf_slist, init)
{
    struct wf_slist list;
    wf_slist_init(&list);

    ASSERT_EQ(nullptr, list.head.next);
    ASSERT_EQ(nullptr, list.last->next);
    ASSERT_EQ(&list.head, list.last);
    ASSERT_TRUE(wf_slist_empty(&list));
    ASSERT_EQ(nullptr, wf_slist_first(&list));    
}

TEST(wf_slist, append)
{
    struct wf_slist list;
    struct wf_slist_item item[3];

    wf_slist_init(&list);
    ASSERT_TRUE(wf_slist_empty(&list));

    wf_slist_append(&list, &item[0]);
    ASSERT_NE(&list.head, list.last);
    ASSERT_FALSE(wf_slist_empty(&list));
    ASSERT_EQ(&item[0], wf_slist_first(&list));    
    ASSERT_EQ(&item[0], list.head.next);
    ASSERT_EQ(&item[0], list.last);
    ASSERT_EQ(nullptr, list.last->next);
    ASSERT_EQ(nullptr, item[0].next);

    wf_slist_append(&list, &item[1]);
    ASSERT_NE(&list.head, list.last);
    ASSERT_FALSE(wf_slist_empty(&list));
    ASSERT_EQ(&item[0], wf_slist_first(&list));    
    ASSERT_EQ(&item[0], list.head.next);
    ASSERT_EQ(&item[1], list.last);
    ASSERT_EQ(nullptr, list.last->next);
    ASSERT_EQ(&item[1], item[0].next);
    ASSERT_EQ(nullptr, item[1].next);

    wf_slist_append(&list, &item[2]);
    ASSERT_NE(&list.head, list.last);
    ASSERT_FALSE(wf_slist_empty(&list));
    ASSERT_EQ(&item[0], wf_slist_first(&list));    
    ASSERT_EQ(&item[0], list.head.next);
    ASSERT_EQ(&item[2], list.last);
    ASSERT_EQ(nullptr, list.last->next);
    ASSERT_EQ(&item[1], item[0].next);
    ASSERT_EQ(&item[2], item[1].next);
    ASSERT_EQ(nullptr, item[2].next);
}

TEST(wf_slist_remove_after, remove_first)
{
    struct wf_slist list;
    struct wf_slist_item item[3];

    wf_slist_init(&list);
    wf_slist_append(&list, &item[0]);
    wf_slist_append(&list, &item[1]);
    wf_slist_append(&list, &item[2]);

    wf_slist_item * removed;
    
    removed = wf_slist_remove_first(&list);
    ASSERT_FALSE(wf_slist_empty(&list));
    ASSERT_EQ(&item[0], removed);

    removed = wf_slist_remove_first(&list);
    ASSERT_FALSE(wf_slist_empty(&list));
    ASSERT_EQ(&item[1], removed);

    removed = wf_slist_remove_first(&list);
    ASSERT_TRUE(wf_slist_empty(&list));
    ASSERT_EQ(&item[2], removed);

    ASSERT_EQ(nullptr, list.head.next);
    ASSERT_EQ(nullptr, list.last->next);
    ASSERT_EQ(&list.head, list.last);
    ASSERT_EQ(nullptr, wf_slist_first(&list));    
}

TEST(wf_slist_remove_after, remove_last)
{
    struct wf_slist list;
    struct wf_slist_item item[3];

    wf_slist_init(&list);
    wf_slist_append(&list, &item[0]);
    wf_slist_append(&list, &item[1]);
    wf_slist_append(&list, &item[2]);

    wf_slist_item * removed;
    
    removed = wf_slist_remove_after(&list, &item[1]);
    ASSERT_FALSE(wf_slist_empty(&list));
    ASSERT_EQ(&item[2], removed);

    removed = wf_slist_remove_after(&list, &item[0]);
    ASSERT_FALSE(wf_slist_empty(&list));
    ASSERT_EQ(&item[1], removed);

    removed = wf_slist_remove_after(&list, &list.head);
    ASSERT_TRUE(wf_slist_empty(&list));
    ASSERT_EQ(&item[0], removed);

    ASSERT_EQ(nullptr, list.head.next);
    ASSERT_EQ(nullptr, list.last->next);
    ASSERT_EQ(&list.head, list.last);
    ASSERT_EQ(nullptr, wf_slist_first(&list));    
}

TEST(wf_slist_remove_after, remove_after)
{
    struct wf_slist list;
    struct wf_slist_item item[3];

    wf_slist_init(&list);
    wf_slist_append(&list, &item[0]);
    wf_slist_append(&list, &item[1]);
    wf_slist_append(&list, &item[2]);

    wf_slist_item * removed;
    
    removed = wf_slist_remove_after(&list, &item[0]);
    ASSERT_FALSE(wf_slist_empty(&list));
    ASSERT_EQ(&item[1], removed);

    ASSERT_NE(&list.head, list.last);
    ASSERT_FALSE(wf_slist_empty(&list));
    ASSERT_EQ(&item[0], wf_slist_first(&list));    
    ASSERT_EQ(&item[0], list.head.next);
    ASSERT_EQ(&item[2], list.last);
    ASSERT_EQ(nullptr, list.last->next);
    ASSERT_EQ(&item[2], item[0].next);
    ASSERT_EQ(nullptr, item[2].next);
}
