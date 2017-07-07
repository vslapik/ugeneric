#include "generic.h"
#include "ut_utils.h"

int main(int argc, char **argv)
{
    unsigned int seed;
    bool verbose = false;

    if (argc >= 2)
    {
        seed = atol(argv[1]);
        ugeneric_random_init_with_seed(seed);
    }
    else
    {
        seed = ugeneric_random_init();
    }

    printf("================================== %u ==============================\n", seed);
    ugeneric_t rv = gen_random_vector(16, verbose);
    uvector_t *vector = G_AS_PTR(rv);
    printf("Generation done ================== %u ==============================\n", seed);

    uvector_sort(G_AS_PTR(rv));
    UASSERT(uvector_is_sorted(G_AS_PTR(rv)));
    printf("Sorting done ===================== %u ==============================\n", seed);

    char *t1 = ugeneric_as_str(rv);
    printf("Serialization done================ %u ==============================\n", seed);

    ugeneric_t rv_copy = ugeneric_copy(rv, NULL);
    uvector_sort(G_AS_PTR(rv_copy));
    UASSERT(uvector_is_sorted(G_AS_PTR(rv_copy)));
    printf("Copy is ready ==================== %u ==============================\n", seed);

    char *t2 = ugeneric_as_str(rv_copy);
    printf("Copy serialization done=========== %u ==============================\n", seed);

    UASSERT(ugeneric_compare(rv, rv_copy, uvector_get_void_comparator(vector)) == 0);

    ufree(t1);
    ufree(t2);

    ugeneric_destroy(rv_copy, NULL);
    ugeneric_destroy(rv, NULL);
    printf("Destoyed ========================= %u ==============================\n", seed);
}
