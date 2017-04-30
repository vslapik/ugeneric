#include <stdio.h>
#include <stdlib.h>
#include "ut_utils.h"
#include "queue.h"

int main(void)
{
    //
    uqueue_t *q = uqueue_create();
    uqueue_reserve_capacity(q, 1000);
    UASSERT(uqueue_get_capacity(q) >= 1000);
    uqueue_destroy(q);

    //
    q = uqueue_create();
    uqueue_enq(q, G_INT(0));
    uqueue_enq(q, G_INT(1));
    uqueue_enq(q, G_INT(2));
    uqueue_enq(q, G_INT(3));
    uqueue_enq(q, G_STR("4"));
    UASSERT_INT_EQ(G_AS_INT(uqueue_deq(q)), 0);
    UASSERT_INT_EQ(G_AS_INT(uqueue_deq(q)), 1);
    UASSERT_INT_EQ(G_AS_INT(uqueue_deq(q)), 2);
    UASSERT_INT_EQ(G_AS_INT(uqueue_deq(q)), 3);
    UASSERT_STR_EQ(G_AS_STR(uqueue_deq(q)), "4");

    //
    UASSERT(uqueue_is_empty(q));
    uqueue_enq(q, G_INT(0));
    UASSERT_INT_EQ(G_AS_INT(uqueue_deq(q)), 0);

    //
    for (size_t i = 0; i < 100; i++)
    {
        uqueue_enq(q, G_INT(i));
    }

    for (size_t i = 0; i < 100; i++)
    {
        UASSERT_INT_EQ(G_AS_INT(uqueue_deq(q)), i);
    }
    UASSERT(uqueue_is_empty(q));

    //
    uqueue_enq(q, G_INT(0));
    UASSERT_INT_EQ(G_AS_INT(uqueue_deq(q)), 0);
    UASSERT(uqueue_is_empty(q));

    uqueue_destroy(q);

    return 0;
}
