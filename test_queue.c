#include <stdio.h>
#include <stdlib.h>
#include "ut_utils.h"
#include "queue.h"

int main(void)
{
    //
    queue_t *q = queue_create();
    queue_reserve_capacity(q, 1000);
    ASSERT(queue_get_capacity(q) >= 1000);
    queue_destroy(q);

    //
    q = queue_create();
    queue_enq(q, G_INT(0));
    queue_enq(q, G_INT(1));
    queue_enq(q, G_INT(2));
    queue_enq(q, G_INT(3));
    queue_enq(q, G_STR("4"));
    ASSERT_INT_EQ(G_AS_INT(queue_deq(q)), 0);
    ASSERT_INT_EQ(G_AS_INT(queue_deq(q)), 1);
    ASSERT_INT_EQ(G_AS_INT(queue_deq(q)), 2);
    ASSERT_INT_EQ(G_AS_INT(queue_deq(q)), 3);
    ASSERT_STR_EQ(G_AS_STR(queue_deq(q)), "4");

    //
    ASSERT(queue_is_empty(q));
    queue_enq(q, G_INT(0));
    ASSERT_INT_EQ(G_AS_INT(queue_deq(q)), 0);

    //
    for (size_t i = 0; i < 100; i++)
    {
        queue_enq(q, G_INT(i));
    }

    for (size_t i = 0; i < 100; i++)
    {
        ASSERT_INT_EQ(G_AS_INT(queue_deq(q)), i);
    }
    ASSERT(queue_is_empty(q));

    //
    queue_enq(q, G_INT(0));
    ASSERT_INT_EQ(G_AS_INT(queue_deq(q)), 0);
    ASSERT(queue_is_empty(q));

    queue_destroy(q);

    return 0;
}
