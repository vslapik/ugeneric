#include "heap.h"

#include "asserts.h"
#include "file_utils.h"
#include "ut_utils.h"

void test_uheap_api(void)
{
    uheap_t *h = uheap_create();
    UASSERT(uheap_is_empty(h));

    uheap_push(h, G_INT(1));
    UASSERT(uheap_get_size(h) == 1);
    UASSERT(!uheap_is_empty(h));

    uheap_clear(h);
    UASSERT(uheap_is_empty(h));
    uheap_destroy(h);

    h = uheap_create();
    uheap_push(h, G_TRUE());
    UASSERT(G_IS_TRUE(uheap_peek(h)));
    UASSERT(!uheap_is_empty(h));
    UASSERT(G_IS_TRUE(uheap_pop(h)));
    UASSERT(uheap_is_empty(h));
    uheap_destroy(h);

    // Corner cases
    h = uheap_create();

    uheap_push(h, G_CSTR("a"));
    UASSERT_STR_EQ(G_AS_STR(uheap_peek(h)), "a");
    uheap_push(h, G_CSTR("b"));
    UASSERT_STR_EQ(G_AS_STR(uheap_peek(h)), "a");
    uheap_push(h, G_CSTR("c"));
    UASSERT_STR_EQ(G_AS_STR(uheap_peek(h)), "a");
    uheap_clear(h);

    uheap_push(h, G_CSTR("b"));
    UASSERT_STR_EQ(G_AS_STR(uheap_peek(h)), "b");
    uheap_push(h, G_CSTR("a"));
    UASSERT_STR_EQ(G_AS_STR(uheap_peek(h)), "a");
    uheap_push(h, G_CSTR("c"));
    UASSERT_STR_EQ(G_AS_STR(uheap_peek(h)), "a");
    uheap_clear(h);

    uheap_push(h, G_CSTR("c"));
    UASSERT_STR_EQ(G_AS_STR(uheap_peek(h)), "c");
    uheap_push(h, G_CSTR("b"));
    UASSERT_STR_EQ(G_AS_STR(uheap_peek(h)), "b");
    uheap_push(h, G_CSTR("a"));
    UASSERT_STR_EQ(G_AS_STR(uheap_peek(h)), "a");
    uheap_clear(h);

    uheap_push(h, G_CSTR("c"));
    UASSERT_STR_EQ(G_AS_STR(uheap_peek(h)), "c");
    uheap_push(h, G_CSTR("a"));
    UASSERT_STR_EQ(G_AS_STR(uheap_peek(h)), "a");
    uheap_push(h, G_CSTR("b"));
    UASSERT_STR_EQ(G_AS_STR(uheap_peek(h)), "a");
    uheap_clear(h);

    uheap_destroy(h);

    // Min heap
    h = uheap_create();
    uheap_push(h, G_INT(100));
    UASSERT_INT_EQ(G_AS_INT(uheap_peek(h)), 100);
    uheap_push(h, G_INT(-1));
    UASSERT_INT_EQ(G_AS_INT(uheap_peek(h)), -1);
    uheap_push(h, G_INT(0));
    UASSERT_INT_EQ(G_AS_INT(uheap_peek(h)), -1);
    uheap_push(h, G_INT(1));
    UASSERT_INT_EQ(G_AS_INT(uheap_peek(h)), -1);
    uheap_push(h, G_INT(7));
    uheap_push(h, G_INT(77));
    uheap_push(h, G_INT(88));
    uheap_push(h, G_INT(99));
    uheap_push(h, G_INT(8));
    uheap_push(h, G_INT(1000));
    uheap_push(h, G_INT(9));
    UASSERT_INT_EQ(G_AS_INT(uheap_peek(h)), -1);
    uheap_push(h, G_INT(-32));
    UASSERT_INT_EQ(G_AS_INT(uheap_peek(h)), -32);

    UASSERT_INT_EQ(G_AS_INT(uheap_peek(h)), -32);
    UASSERT_INT_EQ(G_AS_INT(uheap_pop(h)), -32);
    UASSERT_INT_EQ(G_AS_INT(uheap_pop(h)), -1);
    UASSERT_INT_EQ(G_AS_INT(uheap_pop(h)), 0);
    UASSERT_INT_EQ(G_AS_INT(uheap_pop(h)), 1);
    UASSERT_INT_EQ(G_AS_INT(uheap_pop(h)), 7);
    UASSERT_INT_EQ(G_AS_INT(uheap_pop(h)), 8);
    UASSERT_INT_EQ(G_AS_INT(uheap_pop(h)), 9);
    UASSERT_INT_EQ(G_AS_INT(uheap_pop(h)), 77);
    UASSERT_INT_EQ(G_AS_INT(uheap_pop(h)), 88);
    UASSERT_INT_EQ(G_AS_INT(uheap_pop(h)), 99);
    UASSERT_INT_EQ(G_AS_INT(uheap_pop(h)), 100);
    UASSERT_INT_EQ(G_AS_INT(uheap_pop(h)), 1000);
    UASSERT(uheap_is_empty(h));
    uheap_destroy(h);

    // Max heap
    h = uheap_create_ext(1, UHEAP_TYPE_MAX);
    uheap_push(h, G_INT(100));
    UASSERT_INT_EQ(G_AS_INT(uheap_peek(h)), 100);
    uheap_push(h, G_INT(-1));
    UASSERT_INT_EQ(G_AS_INT(uheap_peek(h)), 100);
    uheap_push(h, G_INT(0));
    UASSERT_INT_EQ(G_AS_INT(uheap_peek(h)), 100);
    uheap_push(h, G_INT(1));
    uheap_push(h, G_INT(7));
    uheap_push(h, G_INT(77));
    uheap_push(h, G_INT(88));
    uheap_push(h, G_INT(99));
    uheap_push(h, G_INT(8));
    UASSERT_INT_EQ(G_AS_INT(uheap_peek(h)), 100);
    uheap_push(h, G_INT(1000));
    UASSERT_INT_EQ(G_AS_INT(uheap_peek(h)), 1000);
    uheap_push(h, G_INT(9));
    UASSERT_INT_EQ(G_AS_INT(uheap_peek(h)), 1000);
    uheap_push(h, G_INT(-32));

    UASSERT_INT_EQ(G_AS_INT(uheap_peek(h)), 1000);
    UASSERT_INT_EQ(G_AS_INT(uheap_pop(h)), 1000);
    UASSERT_INT_EQ(G_AS_INT(uheap_pop(h)), 100);
    UASSERT_INT_EQ(G_AS_INT(uheap_pop(h)), 99);
    UASSERT_INT_EQ(G_AS_INT(uheap_pop(h)), 88);
    UASSERT_INT_EQ(G_AS_INT(uheap_pop(h)), 77);
    UASSERT_INT_EQ(G_AS_INT(uheap_pop(h)), 9);
    UASSERT_INT_EQ(G_AS_INT(uheap_pop(h)), 8);
    UASSERT_INT_EQ(G_AS_INT(uheap_pop(h)), 7);
    UASSERT_INT_EQ(G_AS_INT(uheap_pop(h)), 1);
    UASSERT_INT_EQ(G_AS_INT(uheap_pop(h)), 0);
    UASSERT_INT_EQ(G_AS_INT(uheap_pop(h)), -1);
    UASSERT_INT_EQ(G_AS_INT(uheap_pop(h)), -32);
    UASSERT(uheap_is_empty(h));
    uheap_destroy(h);
}

void test_running_median(void)
{
    ugeneric_t tmp = ufile_read_lines("utdata/median.txt", "\r\n");
    UASSERT_NO_ERROR(tmp);
    uvector_t *v = G_AS_PTR(tmp);

    uheap_t *lheap = uheap_create_ext(5005, UHEAP_TYPE_MAX);
    uheap_t *rheap = uheap_create_ext(5005, UHEAP_TYPE_MIN);

    long sum = 0;
    size_t vlen = uvector_get_size(v);
    for (size_t i = 0; i < vlen; i++)
    {
        ugeneric_t e = ugeneric_parse(G_AS_STR(uvector_get_at(v, i)));
        if (uheap_get_size(lheap))
        {
            ugeneric_t lmax = uheap_peek(lheap);

            if (ugeneric_compare(e, lmax) <= 0)
            {
                uheap_push(lheap, e);
            }
            else
            {
                uheap_push(rheap, e);
            }
        }
        else
        {
            uheap_push(lheap, e);
        }
        size_t lsize = uheap_get_size(lheap);
        size_t rsize = uheap_get_size(rheap);

        if (lsize > rsize + 1)
        {
            uheap_push(rheap, uheap_pop(lheap));
            lsize--; rsize++;
        }
        else if (rsize > lsize + 1)
        {
            uheap_push(lheap, uheap_pop(rheap));
            lsize++; rsize--;
        }

        if (rsize > lsize)
        {
            uheap_push(lheap, uheap_pop(rheap));
        }

        sum += G_AS_INT(uheap_peek(lheap));
        sum %= 10000;
    }
    //printf("sum: %zu\n", sum);
    //UASSERT_INT_EQ(1213, sum);
    UASSERT_INT_EQ(9, sum);

    uvector_destroy(v);
    uheap_destroy(lheap);
    uheap_destroy(rheap);
}

void _check_heap(const uheap_t *h)
{
    if (uheap_is_empty(h))
    {
        return;
    }

    uheap_t *copy = uheap_deep_copy(h);
    ugeneric_t prev = uheap_pop(copy);
    uheap_type_t t = uheap_get_type(copy);
    void_cmp_t cmp = uheap_get_void_comparator(copy);
    while (!uheap_is_empty(copy))
    {
        ugeneric_t e = uheap_pop(copy);
        if (t == UHEAP_TYPE_MAX)
        {
            UASSERT(ugeneric_compare_v(prev, e, cmp) >= 0);
        }
        else if (t == UHEAP_TYPE_MIN)
        {
            UASSERT(ugeneric_compare_v(prev, e, cmp) <= 0);
        }
        else
        {
            UASSERT("impossible happened");
        }
        ugeneric_destroy(prev);
        prev = e;
    }
    ugeneric_destroy(prev);

    uheap_destroy(copy);
}

void check_heap(const ugeneric_t *a, size_t nmemb)
{
    uheap_t *h;

    h = uheap_build_from_array(a, nmemb, UHEAP_TYPE_MAX, NULL);
    _check_heap(h);
    uheap_destroy(h);

    h = uheap_build_from_array(a, nmemb, UHEAP_TYPE_MIN, NULL);
    _check_heap(h);
    uheap_destroy(h);
}

#define JOIN_(x,y) x##y
#define JOIN(x,y) JOIN_(x,y)
#define TMP JOIN(line,__LINE__)

void test_heapify(void)
{
    ugeneric_t TMP[] = {G_INT(777)};                   check_heap(TMP, ARR_LEN(TMP));
    ugeneric_t TMP[] = {G_INT(1), G_INT(2), G_INT(3)}; check_heap(TMP, ARR_LEN(TMP));
    ugeneric_t TMP[] = {G_INT(1), G_INT(3), G_INT(2)}; check_heap(TMP, ARR_LEN(TMP));
    ugeneric_t TMP[] = {G_INT(2), G_INT(1), G_INT(3)}; check_heap(TMP, ARR_LEN(TMP));
    ugeneric_t TMP[] = {G_INT(2), G_INT(3), G_INT(1)}; check_heap(TMP, ARR_LEN(TMP));
    ugeneric_t TMP[] = {G_INT(3), G_INT(1), G_INT(2)}; check_heap(TMP, ARR_LEN(TMP));
    ugeneric_t TMP[] = {G_INT(3), G_INT(2), G_INT(1)}; check_heap(TMP, ARR_LEN(TMP));


    ugeneric_t z[] = {G_INT(103), G_INT(55), G_INT(777), G_INT(-1), G_INT(7),
                      G_INT(1),  G_INT(3),   G_INT(4),  G_INT(10), G_INT(15),
                      G_INT(16), G_INT(100), G_INT(32), G_INT(11), G_INT(2),
                      G_INT(63), G_INT(3),   G_INT(4), G_INT(9999), G_INT(0)};
    check_heap(z, ARR_LEN(z));
}

int main(void)
{
    test_uheap_api();
    test_heapify();
    test_running_median();

    return 0;
}
