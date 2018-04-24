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
    long int A1[] = {1};
    long int B1[] = {1, 2};
    long int B2[] = {2, 1};
    long int B3[] = {2, 2};
    long int C1[] = {1, 2, 3};
    long int C2[] = {1, 3, 2};
    long int C3[] = {2, 1, 3};
    long int C4[] = {2, 3, 1};
    long int C5[] = {3, 1, 2};
    long int C6[] = {3, 2, 1};
    long int  D1[] = {1, 2, 3, 4};
    long int  D2[] = {1, 3, 2, 4};
    long int  D3[] = {2, 1, 3, 4};
    long int  D4[] = {2, 3, 1, 4};
    long int  D5[] = {3, 1, 2, 4};
    long int  D6[] = {3, 2, 1, 4};
    long int  D7[] = {1, 2, 4, 3};
    long int  D8[] = {1, 3, 4, 2};
    long int  D9[] = {2, 1, 4, 3};
    long int D10[] = {2, 3, 4, 1};
    long int D11[] = {3, 1, 4, 2};
    long int D12[] = {3, 2, 4, 1};
    long int D13[] = {1, 4, 2, 3};
    long int D14[] = {1, 4, 3, 2};
    long int D15[] = {2, 4, 1, 3};
    long int D16[] = {2, 4, 3, 1};
    long int D17[] = {3, 4, 1, 2};
    long int D18[] = {3, 4, 2, 1};
    long int D19[] = {4, 1, 2, 3};
    long int D20[] = {4, 1, 3, 2};
    long int D21[] = {4, 2, 1, 3};
    long int D22[] = {4, 2, 3, 1};
    long int D23[] = {4, 3, 1, 2};
    long int D24[] = {4, 3, 2, 1};

    long int b[] = {2, 3, 3, 2, 1, 3, 2, 1, 1, 1, 3, 4, 2, 1, 2, 4, 1, 3};
    long int c[] = {2, 63, 3, 4, 5};
    long int d[] = {1, 3, 4, 10, 15, 16, 100, 32, 11, 2, 63, 3, 4};
    long int e[] = {1, 1, 1, 2, 2};
    long int f[] = {3, 2, 1, 0, -1};
    long int g[] = {-1};
    double h[] = {-0.7, 0.9, 0.3, -0.1};
    char *j[] = {"3", "2", "1"};
    long int o[] = {2, 2, 2, 2, 1};
    long int p[] = {4, 6, 3, 2, 1, 9, 7};
    long int q[] = {4, 3, 3, 1};
    long int r[] = {1, 3, 3, 4};
    long int s[] = {3, 1, 4, 3};
    long int t[] = {0, -6, -5, -4, -3, -2, 6};

    const char *vA1 = "[1]";
    const char *vB1 = "[1, 2]";
    const char *vB2 = "[1, 2]";
    const char *vB3 = "[2, 2]";
    const char *vC1 = "[1, 2, 3]";
    const char *vC2 = "[1, 2, 3]";
    const char *vC3 = "[1, 2, 3]";
    const char *vC4 = "[1, 2, 3]";
    const char *vC5 = "[1, 2, 3]";
    const char *vC6 = "[1, 2, 3]";
    const char  *vD1 = "[1, 2, 3, 4]";
    const char  *vD2 = "[1, 2, 3, 4]";
    const char  *vD3 = "[1, 2, 3, 4]";
    const char  *vD4 = "[1, 2, 3, 4]";
    const char  *vD5 = "[1, 2, 3, 4]";
    const char  *vD6 = "[1, 2, 3, 4]";
    const char  *vD7 = "[1, 2, 3, 4]";
    const char  *vD8 = "[1, 2, 3, 4]";
    const char  *vD9 = "[1, 2, 3, 4]";
    const char *vD10 = "[1, 2, 3, 4]";
    const char *vD11 = "[1, 2, 3, 4]";
    const char *vD12 = "[1, 2, 3, 4]";
    const char *vD13 = "[1, 2, 3, 4]";
    const char *vD14 = "[1, 2, 3, 4]";
    const char *vD15 = "[1, 2, 3, 4]";
    const char *vD16 = "[1, 2, 3, 4]";
    const char *vD17 = "[1, 2, 3, 4]";
    const char *vD18 = "[1, 2, 3, 4]";
    const char *vD19 = "[1, 2, 3, 4]";
    const char *vD20 = "[1, 2, 3, 4]";
    const char *vD21 = "[1, 2, 3, 4]";
    const char *vD22 = "[1, 2, 3, 4]";
    const char *vD23 = "[1, 2, 3, 4]";
    const char *vD24 = "[1, 2, 3, 4]";

    const char *vb = "[1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 4, 4]";
    const char *vc = "[2, 3, 4, 5, 63]";
    const char *vd = "[1, 2, 3, 3, 4, 4, 10, 11, 15, 16, 32, 63, 100]";
    const char *ve = "[1, 1, 1, 2, 2]";
    const char *vf = "[-1, 0, 1, 2, 3]";
    const char *vg = "[-1]";
    const char *vh = "[-0.7, -0.1, 0.3, 0.9]";
    const char *vj = "[\"1\", \"2\", \"3\"]";
    const char *vo = "[1, 2, 2, 2, 2]";
    const char *vp = "[1, 2, 3, 4, 6, 7, 9]";
    const char *vq = "[1, 3, 3, 4]";
    const char *vr = "[1, 3, 3, 4]";
    const char *vs = "[1, 3, 3, 4]";
    const char *vt = "[-6, -5, -4, -3, -2, 0, 6]";

    #define _check(arr, sort, type) {                                  \
        /*puts("sorting "#arr); */                                     \
        uvector_t *v = uvector_create_from_array(arr, ARR_LEN(arr), sizeof(arr[0]), type); \
        sort(uvector_get_cells(v), ARR_LEN(arr), NULL);                \
        char *str = uvector_as_str(v);                                 \
        UASSERT_STR_EQ(str, v##arr);                                   \
        UASSERT(uvector_is_sorted(v));                                 \
        uvector_destroy(v);                                            \
        ufree(str);                                                    \
    }

    _check(A1, sort, G_INT_T);
    _check(B1, sort, G_INT_T);
    _check(B2, sort, G_INT_T);
    _check(B3, sort, G_INT_T);
    _check(C1, sort, G_INT_T);
    _check(C2, sort, G_INT_T);
    _check(C3, sort, G_INT_T);
    _check(C4, sort, G_INT_T);
    _check(C5, sort, G_INT_T);
    _check(C6, sort, G_INT_T);

    _check(D1, sort, G_INT_T);
    _check(D2, sort, G_INT_T);
    _check(D3, sort, G_INT_T);
    _check(D4, sort, G_INT_T);
    _check(D5, sort, G_INT_T);
    _check(D6, sort, G_INT_T);
    _check(D7, sort, G_INT_T);
    _check(D8, sort, G_INT_T);
    _check(D9, sort, G_INT_T);
    _check(D10, sort, G_INT_T);
    _check(D11, sort, G_INT_T);
    _check(D12, sort, G_INT_T);
    _check(D13, sort, G_INT_T);
    _check(D14, sort, G_INT_T);
    _check(D15, sort, G_INT_T);
    _check(D16, sort, G_INT_T);
    _check(D17, sort, G_INT_T);
    _check(D18, sort, G_INT_T);
    _check(D19, sort, G_INT_T);
    _check(D20, sort, G_INT_T);
    _check(D21, sort, G_INT_T);
    _check(D22, sort, G_INT_T);
    _check(D23, sort, G_INT_T);
    _check(D24, sort, G_INT_T);

    _check(b, sort, G_INT_T);
    _check(c, sort, G_INT_T);
    _check(d, sort, G_INT_T);
    _check(e, sort, G_INT_T);
    _check(f, sort, G_INT_T);
    _check(g, sort, G_INT_T);
    _check(h, sort, G_REAL_T);
    _check(j, sort, G_CSTR_T);
    _check(o, sort, G_INT_T);
    _check(p, sort, G_INT_T);
    _check(q, sort, G_INT_T);
    _check(r, sort, G_INT_T);
    _check(s, sort, G_INT_T);
    _check(t, sort, G_INT_T);

    if (sort == quick_sort || sort == hybrid_sort)
    {
        static ugeneric_t fff[10000];
        const char *path = "utdata/array.txt";
        ugeneric_t t = ufile_open(path, "r");
        UASSERT_NO_ERROR(t);
        FILE *fd = G_AS_PTR(t);
        UASSERT(fd);
        for (size_t i = 0; i < ARR_LEN(fff); i++)
        {
            int tmp;
            UASSERT_INT_EQ(fscanf(fd, "%d", &tmp), 1);
            fff[i] = G_INT(tmp);
        }
        t = ufile_close(fd);
        UASSERT_NO_ERROR(t);
        sort(fff, ARR_LEN(fff), NULL);
        ugeneric_array_is_sorted(fff, ARR_LEN(fff), NULL);
    }
}

void test_count_iversions(void)
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
    UASSERT_LLINT_EQ(2407905288, count_inversions(f, ARR_LEN(f), NULL));
}

int main(void)
{
    test_count_iversions();
    test_sort(merge_sort);
    test_sort(insertion_sort);
    test_sort(quick_sort);
    test_sort(hybrid_sort);
    test_sort(selection_sort);
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
