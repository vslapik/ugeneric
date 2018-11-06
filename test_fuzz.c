#include "ut_utils.h"
#include "heap.h"

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

void test_world(bool verbose, unsigned int seed)
{
    printf("================================== %u ============\n", seed);
    ugeneric_t rv = gen_random_vector(7, 50, verbose);
    uvector_t *vector = G_AS_PTR(rv);
    printf("Generation done ================== %u ============\n", seed);
    if (verbose)
    {
        uvector_print(vector);
    }

    uvector_sort(G_AS_PTR(rv));
    UASSERT(uvector_is_sorted(G_AS_PTR(rv)));
    printf("Sorting done ===================== %u ============\n", seed);

    char *t1 = ugeneric_as_str(rv);
    printf("Serialization done================ %u ============\n", seed);

    ugeneric_t rv_copy = ugeneric_copy(rv);
    uvector_sort(G_AS_PTR(rv_copy));
    UASSERT(uvector_is_sorted(G_AS_PTR(rv_copy)));
    printf("Copy is ready ==================== %u ============\n", seed);

    char *t2 = ugeneric_as_str(rv_copy);
    printf("Copy serialization done=========== %u ============\n", seed);

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
    printf("Destoyed ========================= %u ============\n", seed);
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
    test_world(verbose, seed);
}
