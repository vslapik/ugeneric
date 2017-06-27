#include "generic.h"
#include "ut_utils.h"

int main(int argc, char **argv)
{
    unsigned int seed;
    if (argc == 2)
    {
        seed = atol(argv[1]);
        ugeneric_random_init_with_seed(seed);
    }
    else
    {
        seed = ugeneric_random_init();
    }
    printf("==================== %u ======================\n", seed);
    ugeneric_t rv = gen_random_vector(10);
    printf("Generation done ================== %u ==============================\n", seed);
    ugeneric_print(rv);
    printf("Printing done ==================== %u ==============================\n", seed);
    ugeneric_destroy(rv, NULL);
    printf("Destoyed ========================= %u ==============================\n", seed);
}
