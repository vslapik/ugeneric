#include <stdio.h>
#include <limits.h>
#include "mem.h"
#include "vector.h"
#include "string_utils.h"
#include "ut_utils.h"

/* to disable linux overcommit "sysctl vm.overcommit_memory=2" */

/*
bool oom(void *data)
{
    uvector_t *v = data;
    uvector_destroy(v);

    fprintf(stderr, "tryint to clean up things out ...\n");

    // true means retry allocation again
    //return true;
    return false;
}

void test_oom(void)
{
    size_t i = 0;

    // Assuming it is too high to be allocated.
    size_t size = 32LL << 32;

    uvector_t *v = uvector_create();
    uvector_set_void_destroyer(v, ufree);

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
*/

void test_umemdup(void)
{
    char *str = umemdup("string", sizeof("string"));
    UASSERT_STR_EQ(str, "string");
    ufree(str);
}

void test_memchunk(void)
{
    char *data = ustring_dup("1234\xff");
    uvector_t *v = uvector_create();
    umemchunk_t t = {.size = 5, .data = data};
    ugeneric_t gm = G_MEMCHUNK(t.data, t.size);
    uvector_append(v, gm);
    uvector_append(v, ugeneric_copy(gm, NULL));
    uvector_append(v, G_VECTOR(uvector_copy(v)));

    char *str = uvector_as_str(v);
    UASSERT_STR_EQ(str, "[mem:31323334ff, mem:31323334ff, [mem:31323334ff, mem:31323334ff]]");
    ufree(str);
    uvector_destroy(v);
}

int main(void)
{
    test_umemdup();
    test_memchunk();

    //test_oom();
}
