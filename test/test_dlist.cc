#include <gtest/gtest.h>
#include "webfuse/core/dlist.h"
#include "webfuse/core/container_of.h"

namespace
{
    struct Item
    {
        wf_dlist_item item;
        char value;
    };
}

TEST(DList, prepend)
{
    struct wf_dlist list;
    wf_dlist_init(&list);
    ASSERT_EQ(nullptr, list.first);

    Item a;
    a.value = 'a';
    wf_dlist_prepend(&list, &a.item);
    ASSERT_EQ(&a.item, list.first);
    ASSERT_EQ(nullptr, a.item.next);
    ASSERT_EQ(nullptr, a.item.prev);

    Item b;
    b.value = 'b';
    wf_dlist_prepend(&list, &b.item);
    ASSERT_EQ(&b.item, list.first);
    ASSERT_EQ(&a.item, b.item.next);
    ASSERT_EQ(nullptr, b.item.prev);
    ASSERT_EQ(nullptr, a.item.next);
    ASSERT_EQ(&b.item, a.item.prev);

    Item c;
    c.value = 'c';
    wf_dlist_prepend(&list, &c.item);
    ASSERT_EQ(&c.item, list.first);
    ASSERT_EQ(&b.item, c.item.next);
    ASSERT_EQ(nullptr, c.item.prev);
    ASSERT_EQ(&a.item, b.item.next);
    ASSERT_EQ(&c.item, b.item.prev);
    ASSERT_EQ(nullptr, a.item.next);
    ASSERT_EQ(&b.item, a.item.prev);

    wf_dlist_cleanup(&list, nullptr, nullptr);
    ASSERT_EQ(nullptr, list.first);
}