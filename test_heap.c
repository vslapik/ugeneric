#include <stdio.h>
#include <stdlib.h>
#include "heap.h"
#include "ut_utils.h"

void test_heap_api(void)
{
    heap_t *h = heap_create();
    UASSERT(heap_is_empty(h));

    heap_push(h, G_INT(1));
    UASSERT(heap_get_size(h) == 1);
    UASSERT(!heap_is_empty(h));

    heap_clear(h);
    UASSERT(heap_is_empty(h));
    heap_destroy(h);

    h = heap_create();
    heap_push(h, G_TRUE);
    UASSERT(G_IS_TRUE(heap_peek(h)));
    UASSERT(!heap_is_empty(h));
    UASSERT(G_IS_TRUE(heap_pop(h)));
    UASSERT(heap_is_empty(h));
    heap_destroy(h);

    // Corner cases
    h = heap_create();

    heap_push(h, G_CSTR("a"));
    UASSERT_STR_EQ(G_AS_STR(heap_peek(h)), "a");
    heap_push(h, G_CSTR("b"));
    UASSERT_STR_EQ(G_AS_STR(heap_peek(h)), "a");
    heap_push(h, G_CSTR("c"));
    UASSERT_STR_EQ(G_AS_STR(heap_peek(h)), "a");
    heap_clear(h);

    heap_push(h, G_CSTR("b"));
    UASSERT_STR_EQ(G_AS_STR(heap_peek(h)), "b");
    heap_push(h, G_CSTR("a"));
    UASSERT_STR_EQ(G_AS_STR(heap_peek(h)), "a");
    heap_push(h, G_CSTR("c"));
    UASSERT_STR_EQ(G_AS_STR(heap_peek(h)), "a");
    heap_clear(h);

    heap_push(h, G_CSTR("c"));
    UASSERT_STR_EQ(G_AS_STR(heap_peek(h)), "c");
    heap_push(h, G_CSTR("b"));
    UASSERT_STR_EQ(G_AS_STR(heap_peek(h)), "b");
    heap_push(h, G_CSTR("a"));
    UASSERT_STR_EQ(G_AS_STR(heap_peek(h)), "a");
    heap_clear(h);

    heap_push(h, G_CSTR("c"));
    UASSERT_STR_EQ(G_AS_STR(heap_peek(h)), "c");
    heap_push(h, G_CSTR("a"));
    UASSERT_STR_EQ(G_AS_STR(heap_peek(h)), "a");
    heap_push(h, G_CSTR("b"));
    UASSERT_STR_EQ(G_AS_STR(heap_peek(h)), "a");
    heap_clear(h);

    heap_destroy(h);

    // Min heap
    h = heap_create();
    heap_push(h, G_INT(100));
    UASSERT_INT_EQ(G_AS_INT(heap_peek(h)), 100);
    heap_push(h, G_INT(-1));
    UASSERT_INT_EQ(G_AS_INT(heap_peek(h)), -1);
    heap_push(h, G_INT(0));
    UASSERT_INT_EQ(G_AS_INT(heap_peek(h)), -1);
    heap_push(h, G_INT(1));
    UASSERT_INT_EQ(G_AS_INT(heap_peek(h)), -1);
    heap_push(h, G_INT(7));
    heap_push(h, G_INT(77));
    heap_push(h, G_INT(88));
    heap_push(h, G_INT(99));
    heap_push(h, G_INT(8));
    heap_push(h, G_INT(1000));
    heap_push(h, G_INT(9));
    UASSERT_INT_EQ(G_AS_INT(heap_peek(h)), -1);
    heap_push(h, G_INT(-32));
    UASSERT_INT_EQ(G_AS_INT(heap_peek(h)), -32);

    UASSERT_INT_EQ(G_AS_INT(heap_peek(h)), -32);
    UASSERT_INT_EQ(G_AS_INT(heap_pop(h)), -32);
    UASSERT_INT_EQ(G_AS_INT(heap_pop(h)), -1);
    UASSERT_INT_EQ(G_AS_INT(heap_pop(h)), 0);
    UASSERT_INT_EQ(G_AS_INT(heap_pop(h)), 1);
    UASSERT_INT_EQ(G_AS_INT(heap_pop(h)), 7);
    UASSERT_INT_EQ(G_AS_INT(heap_pop(h)), 8);
    UASSERT_INT_EQ(G_AS_INT(heap_pop(h)), 9);
    UASSERT_INT_EQ(G_AS_INT(heap_pop(h)), 77);
    UASSERT_INT_EQ(G_AS_INT(heap_pop(h)), 88);
    UASSERT_INT_EQ(G_AS_INT(heap_pop(h)), 99);
    UASSERT_INT_EQ(G_AS_INT(heap_pop(h)), 100);
    UASSERT_INT_EQ(G_AS_INT(heap_pop(h)), 1000);
    UASSERT(heap_is_empty(h));
    heap_destroy(h);

    // Max heap
    h = heap_create_ext(1, MAX_HEAP);
    heap_push(h, G_INT(100));
    UASSERT_INT_EQ(G_AS_INT(heap_peek(h)), 100);
    heap_push(h, G_INT(-1));
    UASSERT_INT_EQ(G_AS_INT(heap_peek(h)), 100);
    heap_push(h, G_INT(0));
    UASSERT_INT_EQ(G_AS_INT(heap_peek(h)), 100);
    heap_push(h, G_INT(1));
    heap_push(h, G_INT(7));
    heap_push(h, G_INT(77));
    heap_push(h, G_INT(88));
    heap_push(h, G_INT(99));
    heap_push(h, G_INT(8));
    UASSERT_INT_EQ(G_AS_INT(heap_peek(h)), 100);
    heap_push(h, G_INT(1000));
    UASSERT_INT_EQ(G_AS_INT(heap_peek(h)), 1000);
    heap_push(h, G_INT(9));
    UASSERT_INT_EQ(G_AS_INT(heap_peek(h)), 1000);
    heap_push(h, G_INT(-32));

    UASSERT_INT_EQ(G_AS_INT(heap_peek(h)), 1000);
    UASSERT_INT_EQ(G_AS_INT(heap_pop(h)), 1000);
    UASSERT_INT_EQ(G_AS_INT(heap_pop(h)), 100);
    UASSERT_INT_EQ(G_AS_INT(heap_pop(h)), 99);
    UASSERT_INT_EQ(G_AS_INT(heap_pop(h)), 88);
    UASSERT_INT_EQ(G_AS_INT(heap_pop(h)), 77);
    UASSERT_INT_EQ(G_AS_INT(heap_pop(h)), 9);
    UASSERT_INT_EQ(G_AS_INT(heap_pop(h)), 8);
    UASSERT_INT_EQ(G_AS_INT(heap_pop(h)), 7);
    UASSERT_INT_EQ(G_AS_INT(heap_pop(h)), 1);
    UASSERT_INT_EQ(G_AS_INT(heap_pop(h)), 0);
    UASSERT_INT_EQ(G_AS_INT(heap_pop(h)), -1);
    UASSERT_INT_EQ(G_AS_INT(heap_pop(h)), -32);
    UASSERT(heap_is_empty(h));
    heap_destroy(h);
}

int main(void)
{
    test_heap_api();

    return 0;
}
