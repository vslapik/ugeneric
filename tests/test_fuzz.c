#include "heap.h"
#include "string_utils.h"
#include "ut_utils.h"

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

void test_heap(bool verbose, unsigned int seed)
{
    printf("====================== %s[%u] =============\n", __func__, seed);
    ugeneric_t g = gen_random_vector(1, 10000, verbose);
    uvector_t *v = G_AS_PTR(g);
    size_t len = uvector_get_size(v);

    ugeneric_t *array = uvector_get_cells(v);
    check_heap(array, len);

    printf("====================== %s[%u] =============\n", __func__, seed);

    uvector_destroy(v);
}

void _check_list(ulist_t *l)
{
    ulist_t *l_copy = ulist_copy(l);
    ulist_reverse(l_copy);

    ulist_iterator_t *li = ulist_iterator_create(l);
    ulist_iterator_t *li_rev = ulist_iterator_create_rev(l_copy);

    while (ulist_iterator_has_next(li))
    {
        UASSERT(ulist_iterator_has_next(li_rev));
        ugeneric_t g1 = ulist_iterator_get_next(li);
        ugeneric_t g2 = ulist_iterator_get_next(li_rev);
        UASSERT(ugeneric_compare_v(g1, g2, ulist_get_void_comparator(l)) == 0);
    }
    UASSERT(!ulist_iterator_has_next(li_rev));

    ulist_reverse(l_copy);
    UASSERT(ulist_compare(l, l_copy, ulist_get_void_comparator(l)) == 0);

    ulist_iterator_destroy(li);
    ulist_iterator_destroy(li_rev);
    ulist_destroy(l_copy);
}

void test_list(bool verbose, unsigned int seed)
{
    printf("====================== %s[%u] =============\n", __func__, seed);
    for (int i = 0; i < 100000; i++)
    {
        int j;
        ulist_t *l = gen_random_list(false);
        size_t size = ulist_get_size(l);

        // insert_at
        if (size)
        {
            j = ugeneric_random_from_range(0, size - 1);
            ulist_insert_at(l, j, gen_random_string(0, false));
            UASSERT_SIZE_EQ(ulist_get_size(l), size + 1);
            _check_list(l);
        }

        // append & prepend
        size = ulist_get_size(l);
        j = ugeneric_random_from_range(0, 1);
        ugeneric_t str = G_STR(ustring_fmt("%s", j ? "appendix" : "prependix"));
        (j ? ulist_append : ulist_prepend)(l, str);
        UASSERT_SIZE_EQ(ulist_get_size(l), size + 1);
        _check_list(l);

        // remove
        size = ulist_get_size(l);
        j = ugeneric_random_from_range(0, size - 1);
        ulist_remove_at(l, j);
        UASSERT_SIZE_EQ(ulist_get_size(l), size - 1);
        _check_list(l);

        // remove_back & remove_front
        size = ulist_get_size(l);
        if (size)
        {
            j = ugeneric_random_from_range(0, size - 1);
            (j ? ulist_remove_back : ulist_remove_front)(l);
            UASSERT_SIZE_EQ(ulist_get_size(l), size - 1);
            _check_list(l);
        }

        if (verbose)
        {
            ulist_print(l);
        }

        ulist_destroy(l);
    }
    printf("====================== %s[%u] =============\n", __func__, seed);
}

void test_world(bool verbose, unsigned int seed)
{
    printf("====================== %s[%u] ============\n", __func__, seed);
    ugeneric_t rv = gen_random_vector(7, 50, verbose);
    uvector_t *vector = G_AS_PTR(rv);
    printf("Generation done ================= %u ============\n", seed);
    if (verbose)
    {
        uvector_print(vector);
    }

    uvector_sort(G_AS_PTR(rv));
    UASSERT(uvector_is_sorted(G_AS_PTR(rv)));
    printf("Sorting done ==================== %u ============\n", seed);

    char *t1 = ugeneric_as_str(rv);
    printf("Serialization done ============== %u ============\n", seed);

    ugeneric_t rv_copy = ugeneric_copy(rv);
    uvector_sort(G_AS_PTR(rv_copy));
    UASSERT(uvector_is_sorted(G_AS_PTR(rv_copy)));
    printf("Copy is ready =================== %u ============\n", seed);

    char *t2 = ugeneric_as_str(rv_copy);
    printf("Copy serialization done ========= %u ============\n", seed);

    UASSERT(ugeneric_compare_v(rv, rv_copy, uvector_get_void_comparator(vector)) == 0);

    ugeneric_t g = ugeneric_parse(t2);
    if (G_IS_ERROR(g))
    {
        ugeneric_error_print(g);
        ugeneric_error_destroy(g);
        UABORT("test failed");
    }
    UASSERT(G_IS_VECTOR(g));
    uvector_sort(G_AS_PTR(g));
    UASSERT(uvector_is_sorted(G_AS_PTR(g)));

    if (verbose)
    {
        ugeneric_print(rv);
        ugeneric_print(g);
    }
    UASSERT(ugeneric_compare_v(rv, g, uvector_get_void_comparator(vector)) == 0);

    ufree(t1);
    ufree(t2);
    ugeneric_destroy(rv_copy);
    ugeneric_destroy(rv);
    ugeneric_destroy(g);
    printf("Destoyed ======================== %u ============\n", seed);
    printf("====================== %s[%u] ============\n", __func__, seed);
}

int main(int argc, char **argv)
{
    bool verbose = false;
    unsigned int seed = ugeneric_random_init();

    while ((argc > 1) && (argv[1][0] == '-'))
    {
        switch (argv[1][1])
        {
            case 'v':
                verbose = true;
                break;

            case 's':
                printf("seed is: %s\n", argv[2]);
                seed = atol(argv[2]);
                ++argv;
                --argc;
                break;

            default:
                printf("Wrong Argument: %s\n", argv[1]);
                //usage();
        }

        ++argv;
        --argc;
    }
    ugeneric_random_init_with_seed(seed);

    test_heap(verbose, seed);
    test_list(verbose, seed);
    test_world(verbose, seed);
}
