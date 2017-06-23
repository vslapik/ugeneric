#include <stdio.h>
#include <limits.h>
#include "mem.h"
#include "vector.h"
#include "ut_utils.h"

/* to disable linux overcommit "sysctl vm.overcommit_memory=2" */

bool oom(void *data)
{
    uvector_t *v = data;
    uvector_destroy(v);

    exit(0);

    fprintf(stderr, "tryint to clean up things out ...\n");
    uvector_clear(v);

    // true means retry allocation again
    //return true;
    return false;
}

void test_umemdup(void)
{
    char *str = umemdup("string", 5);
    UASSERT_STR_EQ(str, "string");
    ufree(str);
}

int main(void)
{
    size_t i = 0;

    // Assuming it is too high to be allocated.
    size_t size = 32LL << 32;

    uvector_t *v = uvector_create();
    uvector_set_destroyer(v, ufree);

    libugeneric_set_oom_handler(oom, v);

    while (true)
    {
        char *p = umalloc(size);
        uvector_append(v, G_PTR(p));
        for (size_t j = 0; j < size; j++)
        {
            p[j] = 1;
        }
        printf("%zu\n", i++);
    }

}
