#include <stdio.h>
#include <limits.h>
#include "mem.h"
#include "vector.h"

/* to disable linux overcommit "sysctl vm.overcommit_memory=2" */

bool oom(void *data)
{
    vector_t *v = data;
    vector_destroy(v);

    exit(0);

    fprintf(stderr, "tryint to clean up things out ...\n");
    vector_clear(v);

    //return true; // true mean retry allocation again
    return false; // true mean retry allocation again
}

int main(void)
{
    size_t i = 0;

    // Assuming it is too high to be allocated.
    size_t size = 32LL << 32;

    vector_t *v = vector_create_empty();
    vector_set_destroyer(v, ufree);

    libugeneric_set_oom_handler(oom, v);

    while (true)
    {
        char *p = umalloc(size);
        vector_append(v, G_PTR(p));
        for (size_t j = 0; j < size; j++)
        {
            p[j] = 1;
        }
        printf("%zu\n", i++);
    }

}
