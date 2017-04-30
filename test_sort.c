#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "mem.h"
#include "sort.h"
#include "vector.h"
#include "generic.h"
#include "file_utils.h"
#include "ut_utils.h"

void print_array(ugeneric_t *base, size_t nmemb);
typedef void (*sort_func)(ugeneric_t *base, size_t nmemb, void_cmp_t cmp);

void test_sort(sort_func sort)
{
    long int a[] = {4, 2};
    long int b[] = {4, 1, 2};
    long int c[] = {2, 63, 3, 4, 5};
    long int d[] = {1, 3, 4, 10, 15, 16, 100, 32, 11, 2, 63, 3, 4};
    long int e[] = {1, 1, 1, 2, 2};
    long int f[] = {3, 2, 1, 0, -1};
    long int g[] = {-1};
    double h[] = {-0.7, 0.9, 0.3, -0.1};

    const char *va = "[2, 4]";
    const char *vb = "[1, 2, 4]";
    const char *vc = "[2, 3, 4, 5, 63]";
    const char *vd = "[1, 2, 3, 3, 4, 4, 10, 11, 15, 16, 32, 63, 100]";
    const char *ve = "[1, 1, 1, 2, 2]";
    const char *vf = "[-1, 0, 1, 2, 3]";
    const char *vg = "[-1]";
    const char *vh = "[-0.7, -0.1, 0.3, 0.9]";

    #define check_on_array(arr, sort, type) {                          \
        /*puts("sorting "#arr); */                                     \
        uvector_t *v = uvector_create_from_array(arr, ARR_LEN(arr), sizeof(arr[0]), type); \
        sort(uvector_get_cells(v), ARR_LEN(arr), NULL);                \
        char *str = uvector_as_str(v);                                 \
        UASSERT_STR_EQ(str, v##arr);                                   \
        uvector_destroy(v);                                            \
        ufree(str);                                                    \
    }

    check_on_array(a, sort, G_INT_T);
    check_on_array(b, sort, G_INT_T);
    check_on_array(c, sort, G_INT_T);
    check_on_array(d, sort, G_INT_T);
    check_on_array(e, sort, G_INT_T);
    check_on_array(f, sort, G_INT_T);
    check_on_array(g, sort, G_INT_T);
    check_on_array(h, sort, G_REAL_T);
}

void test_count_iversions()
{
    ugeneric_t a[] = {G_INT(4), G_INT(2)};
    ugeneric_t b[] = {G_INT(2), G_INT(63), G_INT(3), G_INT(4), G_INT(5)};
    ugeneric_t c[] = {
        G_INT(1),  G_INT(3),   G_INT(4),  G_INT(10), G_INT(15),
        G_INT(16), G_INT(100), G_INT(32), G_INT(11), G_INT(2),
        G_INT(63), G_INT(3),   G_INT(4)
    };
    ugeneric_t d[] = {G_INT(1), G_INT(1), G_INT(1), G_INT(2), G_INT(2)};
    ugeneric_t e[] = {G_INT(3), G_INT(2), G_INT(1), G_INT(0), G_INT(-1)};

    UASSERT_INT_EQ(1, count_inversions(a, ARR_LEN(a), NULL));
    UASSERT_INT_EQ(3, count_inversions(b, ARR_LEN(b), NULL));
    UASSERT_INT_EQ(29, count_inversions(c, ARR_LEN(c), NULL));
    UASSERT_INT_EQ(0, count_inversions(d, ARR_LEN(d), NULL));
    UASSERT_INT_EQ(10, count_inversions(e, ARR_LEN(e), NULL));

    ugeneric_t f[100000];
    const char *path = "utdata/array.txt";
    ugeneric_t t = ufile_open(path, "r");
    UASSERT_NO_ERROR(t);
    FILE *fd = G_AS_PTR(t);
    UASSERT(fd);
    for (size_t i = 0; i < ARR_LEN(f); i++)
    {
        int tmp;
        UASSERT_INT_EQ(fscanf(fd, "%d", &tmp), 1);
        f[i] = G_INT(tmp);
    }
    t = ufile_close(fd);
    UASSERT_NO_ERROR(t);
    UASSERT_INT_EQ(2407905288, count_inversions(f, ARR_LEN(f), NULL));
}

int main(void)
{
    test_count_iversions();
    test_sort(quick_sort);
    test_sort(merge_sort);
    test_sort(insertion_sort);
    //test_sort(selection_sort);
}

void print_array(ugeneric_t *base, size_t nmemb)
{
    printf("[ ");
    for (size_t i = 0; i < nmemb; i++)
    {
        printf("%ld ", G_AS_INT(base[i]));
    }
    printf("]\n");
}
