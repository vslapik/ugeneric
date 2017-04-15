#include <stdio.h>
#include <stdlib.h>
#include "heap.h"
#include "ut_utils.h"

void test_heap_api(void)
{
    heap_t *h = heap_create();
    ASSERT(heap_is_empty(h));

    heap_push(h, G_INT(1));
    ASSERT(heap_get_size(h) == 1);
    ASSERT(!heap_is_empty(h));

    heap_clear(h);
    ASSERT(heap_is_empty(h));

    heap_push(h, G_INT(100));
    heap_push(h, G_INT(0));
    heap_push(h, G_INT(1));
    heap_push(h, G_INT(7));
    heap_push(h, G_INT(77));
    heap_push(h, G_INT(88));
    heap_push(h, G_INT(99));
    heap_push(h, G_INT(8));
    heap_push(h, G_INT(1000));
    heap_push(h, G_INT(9));
    heap_push(h, G_INT(-32));

    ASSERT_INT_EQ(G_AS_INT(heap_peek(h)), -32);
    ASSERT_INT_EQ(G_AS_INT(heap_pop(h)), -32);
    ASSERT_INT_EQ(G_AS_INT(heap_pop(h)), 0);
    ASSERT_INT_EQ(G_AS_INT(heap_pop(h)), 1);
    ASSERT_INT_EQ(G_AS_INT(heap_pop(h)), 7);
    ASSERT_INT_EQ(G_AS_INT(heap_pop(h)), 8);
    ASSERT_INT_EQ(G_AS_INT(heap_pop(h)), 9);
    ASSERT_INT_EQ(G_AS_INT(heap_pop(h)), 77);
    ASSERT_INT_EQ(G_AS_INT(heap_pop(h)), 88);
    ASSERT_INT_EQ(G_AS_INT(heap_pop(h)), 99);
    ASSERT_INT_EQ(G_AS_INT(heap_pop(h)), 100);
    ASSERT_INT_EQ(G_AS_INT(heap_pop(h)), 1000);
    ASSERT(heap_is_empty(h));

    heap_destroy(h);

    h = heap_create();
    heap_push(h, G_TRUE);
    ASSERT(G_IS_TRUE(heap_peek(h)));
    ASSERT(!heap_is_empty(h));
    ASSERT(G_IS_TRUE(heap_pop(h)));
    ASSERT(heap_is_empty(h));
    heap_destroy(h);

}

int main(void)
{
    test_heap_api();

    return 0;
}