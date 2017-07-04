#include <stdio.h>
#include <stdlib.h>
#include "vector.h"
#include "void.h"
#include "mem.h"
#include "string_utils.h"
#include "ut_utils.h"

typedef struct {
    char *name;
    char age;
} nage_t;

void free_nage_t(void *pnage)
{
    nage_t *nage = pnage;
    ufree(nage->name);
    ufree(nage);
}

void *copy_nage_t(const void *pnage)
{
    const nage_t *nage = pnage;

    nage_t *n = umalloc(sizeof(*n));
    UASSERT(n);

    n->name = ustring_dup(nage->name);
    n->age = nage->age;

    return n;
}

void test_uvector_copy(bool verbose)
{
    (void)verbose;

    uvector_t *v = uvector_create();
    uvector_set_void_destroyer(v, free_nage_t);
    uvector_set_void_copier(v, copy_nage_t);

    nage_t *n = umalloc(sizeof(*n));

    *n = (nage_t){ustring_dup("john"), 34};
    uvector_append(v, G_PTR(n)); // v owns *n data

    uvector_t *v2 = uvector_copy(v); // v2 has shallow copy of *n2
    uvector_t *v3 = uvector_deep_copy(v); // v3 has deep copy of *n2, must set own

    uvector_destroy(v);
    uvector_destroy(v2);
    uvector_destroy(v3);
}

void test_uvector_bsearch(void)
{
    uvector_t *v = uvector_create();
    uvector_append(v, G_INT(1));
    uvector_append(v, G_INT(2));
    uvector_append(v, G_INT(4));
    uvector_append(v, G_INT(3));

    uvector_sort(v); // [1, 2, 3, 4]]
    UASSERT_INT_EQ(uvector_bsearch(v, G_INT(1)), 0);
    UASSERT_INT_EQ(uvector_bsearch(v, G_INT(2)), 1);
    UASSERT_INT_EQ(uvector_bsearch(v, G_INT(3)), 2);
    UASSERT_INT_EQ(uvector_bsearch(v, G_INT(4)), 3);
    UASSERT_INT_EQ(uvector_bsearch(v, G_INT(400)), SIZE_MAX);
    UASSERT_INT_EQ(uvector_bsearch(v, G_INT(-400)), SIZE_MAX);

    uvector_remove_at(v, 1); // [1, 3, 4]
    UASSERT_INT_EQ(uvector_bsearch(v, G_INT(1)), 0);
    UASSERT_INT_EQ(uvector_bsearch(v, G_INT(2)), SIZE_MAX);
    UASSERT_INT_EQ(uvector_bsearch(v, G_INT(3)), 1);
    UASSERT_INT_EQ(uvector_bsearch(v, G_INT(4)), 2);
    UASSERT_INT_EQ(uvector_bsearch(v, G_INT(400)), SIZE_MAX);
    UASSERT_INT_EQ(uvector_bsearch(v, G_INT(-400)), SIZE_MAX);

    uvector_pop_back(v); // [1, 3]
    UASSERT_INT_EQ(uvector_bsearch(v, G_INT(1)), 0);
    UASSERT_INT_EQ(uvector_bsearch(v, G_INT(2)), SIZE_MAX);
    UASSERT_INT_EQ(uvector_bsearch(v, G_INT(3)), 1);
    UASSERT_INT_EQ(uvector_bsearch(v, G_INT(4)), SIZE_MAX);
    UASSERT_INT_EQ(uvector_bsearch(v, G_INT(400)), SIZE_MAX);
    UASSERT_INT_EQ(uvector_bsearch(v, G_INT(-400)), SIZE_MAX);

    uvector_pop_at(v, 0); // [3]
    UASSERT_INT_EQ(uvector_bsearch(v, G_INT(1)), SIZE_MAX);
    UASSERT_INT_EQ(uvector_bsearch(v, G_INT(2)), SIZE_MAX);
    UASSERT_INT_EQ(uvector_bsearch(v, G_INT(3)), 0);
    UASSERT_INT_EQ(uvector_bsearch(v, G_INT(4)), SIZE_MAX);
    UASSERT_INT_EQ(uvector_bsearch(v, G_INT(400)), SIZE_MAX);
    UASSERT_INT_EQ(uvector_bsearch(v, G_INT(-400)), SIZE_MAX);

    uvector_pop_at(v, 0);
    UASSERT_INT_EQ(uvector_get_size(v), 0);
    UASSERT_INT_EQ(uvector_bsearch(v, G_INT(1)), SIZE_MAX);
    UASSERT_INT_EQ(uvector_bsearch(v, G_INT(2)), SIZE_MAX);
    UASSERT_INT_EQ(uvector_bsearch(v, G_INT(3)), SIZE_MAX);
    UASSERT_INT_EQ(uvector_bsearch(v, G_INT(4)), SIZE_MAX);
    UASSERT_INT_EQ(uvector_bsearch(v, G_INT(400)), SIZE_MAX);
    UASSERT_INT_EQ(uvector_bsearch(v, G_INT(-400)), SIZE_MAX);

    uvector_destroy(v);
}

void test_uvector_next_permutation(bool verbose)
{
    uvector_t *v = uvector_create();
    uvector_append(v, G_INT(4));
    uvector_append(v, G_INT(3));
    uvector_append(v, G_INT(1));
    uvector_append(v, G_INT(2));

    do {
        if (verbose)
            uvector_print(v);
    } while (uvector_next_permutation(v));

    uvector_destroy(v);
}

void test_uvector_serialization(bool verbose)
{
    char *vs;
    uvector_t *v = uvector_create();

    vs = uvector_as_str(v);
    UASSERT_STR_EQ(vs, "[]");
    ufree(vs);

    uvector_append(v, G_INT(11));
    uvector_append(v, G_INT(22));
    uvector_append(v, G_INT(33));
    uvector_append(v, G_STR(ustring_dup("44")));
    uvector_append(v, G_CSTR("5"));

    uvector_append(v, G_VECTOR(uvector_deep_copy(v)));
    uvector_append(v, G_VECTOR(uvector_deep_copy(v)));

    if (verbose)
        uvector_print(v);
    vs = uvector_as_str(v);
    UASSERT_STR_EQ(vs,
                  "[11, 22, 33, \"44\", \"5\", [11, 22, 33, \"44\", \"5\"], [11, 22, 33, \"44\", \"5\", [11, 22, 33, \"44\", \"5\"]]]");
    ufree(vs);
    uvector_destroy(v);
}

void test_uvector_api()
{
    uvector_t *v = uvector_create();
    uvector_append(v, G_INT(0));
    UASSERT(uvector_get_capacity(v) >= VECTOR_INITIAL_CAPACITY);
    uvector_reserve_capacity(v, 1000);
    UASSERT_INT_EQ(uvector_get_capacity(v), 1000);
    uvector_destroy(v);

    v = uvector_create_with_size(3, G_NULL);
    UASSERT_INT_EQ(uvector_get_size(v), 3);

    uvector_append(v, G_INT(11));
    uvector_append(v, G_INT(22));
    uvector_append(v, G_INT(33));
    UASSERT_INT_EQ(uvector_get_size(v), 6);
    UASSERT(G_IS_NULL(uvector_get_at(v, 0)));
    UASSERT_INT_EQ(G_AS_INT(uvector_get_at(v, 3)), 11);
    UASSERT_INT_EQ(G_AS_INT(uvector_get_at(v, 4)), 22);
    UASSERT_INT_EQ(G_AS_INT(uvector_get_at(v, 5)), 33);

    uvector_swap(v, 4, 5);
    UASSERT_INT_EQ(G_AS_INT(uvector_get_at(v, 4)), 33);
    UASSERT_INT_EQ(G_AS_INT(uvector_get_at(v, 5)), 22);

    uvector_resize(v, 0, G_NULL);
    UASSERT(uvector_is_empty(v));

    uvector_append(v, G_INT(44));
    UASSERT_INT_EQ(uvector_get_size(v), 1);
    UASSERT_INT_EQ(G_AS_INT(uvector_get_at(v, 0)), 44);

    uvector_set_at(v, 0, G_INT(2222));
    UASSERT_INT_EQ(G_AS_INT(uvector_get_at(v, 0)), 2222);

    ugeneric_t *a = uvector_get_cells(v);
    UASSERT_INT_EQ(G_AS_INT(a[0]), 2222);
    uvector_destroy(v);

    v = uvector_create();
    UASSERT_INT_EQ(uvector_get_size(v), 0);
    UASSERT(uvector_is_empty(v));
    uvector_append(v, G_INT(11));
    UASSERT(!uvector_is_empty(v));
    UASSERT_INT_EQ(uvector_get_size(v), 1);
    UASSERT_INT_EQ(G_AS_INT(uvector_pop_back(v)), 11);
    UASSERT(uvector_is_empty(v));
    UASSERT_INT_EQ(uvector_get_size(v), 0);

    uvector_resize(v, 0, G_NULL);
    uvector_append(v, G_INT(200));
    uvector_append(v, G_INT(100));
    UASSERT_INT_EQ(uvector_get_size(v), 2);
    UASSERT_INT_EQ(G_AS_INT(uvector_get_at(v, 0)), 200);
    UASSERT_INT_EQ(G_AS_INT(uvector_get_at(v, 1)), 100);
    uvector_insert_at(v, 1, G_INT(300));
    UASSERT_INT_EQ(uvector_get_size(v), 3);
    UASSERT_INT_EQ(G_AS_INT(uvector_get_at(v, 0)), 200);
    UASSERT_INT_EQ(G_AS_INT(uvector_get_at(v, 1)), 300);
    UASSERT_INT_EQ(G_AS_INT(uvector_get_at(v, 2)), 100);
    UASSERT_INT_EQ(G_AS_INT(uvector_pop_at(v, 0)), 200);
    UASSERT_INT_EQ(uvector_get_size(v), 2);
    UASSERT_INT_EQ(G_AS_INT(uvector_pop_at(v, 0)), 300);
    UASSERT_INT_EQ(uvector_get_size(v), 1);
    UASSERT_INT_EQ(G_AS_INT(uvector_pop_at(v, 0)), 100);
    UASSERT_INT_EQ(uvector_get_size(v), 0);
    uvector_resize(v, 1000, G_NULL);
    UASSERT_INT_EQ(uvector_get_size(v), 1000);
    uvector_destroy(v);

    v = uvector_create();
    uvector_append(v, G_STR(ustring_dup("1")));
    uvector_append(v, G_STR(ustring_dup("2")));
    uvector_append(v, G_STR(ustring_dup("3")));
    uvector_append(v, G_STR(ustring_dup("4")));
    uvector_resize(v, 2, G_NULL);
    uvector_resize(v, 1, G_NULL);
    uvector_clear(v);
    uvector_append(v, G_STR(ustring_dup("5")));
    UASSERT_INT_EQ(uvector_get_size(v), 1);
    uvector_destroy(v);

    v = uvector_create();
    uvector_append(v, G_STR(ustring_dup("string")));
    uvector_set_at(v, 0, G_STR(ustring_dup("string2")));
    uvector_destroy(v);

    v = uvector_create();
    uvector_append(v, G_INT(123));
    UASSERT_INT_EQ(G_AS_INT(uvector_get_at(v, 0)), 123);
    UASSERT_INT_EQ(G_AS_INT(uvector_get_back(v)), 123);
    uvector_t *vcopy = uvector_copy(v);
    UASSERT(uvector_compare(v, vcopy, NULL) == 0);
    uvector_destroy(v);
    uvector_destroy(vcopy);

    v = uvector_create();
    uvector_reserve_capacity(v, 1024);
    uvector_shrink_to_size(v);
    uvector_destroy(v);

/*
    UASSERT_UABORTS(uvector_destroy(v));
    UASSERT_UABORTS(uvector_destroy(NULL));
    UASSERT_UABORTS(uvector_append(v, G_INT(0)));
    UASSERT_UABORTS(uvector_pop_back(v));
    UASSERT_UABORTS(uvector_pop_front(v));
    UASSERT_UABORTS(uvector_get_cells(v));
    UASSERT_UABORTS(uvector_get_size(v));
    UASSERT_UABORTS(uvector_resize(v, 100 G_NULL));
    UASSERT_UABORTS(uvector_reserve_capacity(v, 100));
    UASSERT_UABORTS(uvector_get_capacity(v));
    UASSERT_UABORTS(uvector_is_empty(v));
    UASSERT_UABORTS(uvector_swap(v, 1, 2));
    UASSERT_UABORTS(uvector_insert_at(v, 1, G_INT(100)));
    UASSERT_UABORTS(uvector_get_at(v, 1));
    UASSERT_UABORTS(uvector_set_at(v, 1, G_INT(0)));

    v = uvector_create(0);
    UASSERT_UABORTS(uvector_get_at(v, 1));
    UASSERT_UABORTS(uvector_set_at(v, 1, G_INT(0)));

    uvector_resize(v, 100, G_NULL);
    UASSERT_UABORTS(uvector_swap(v, 150, 250));
    UASSERT_UABORTS(uvector_swap(v, 50, 250));
    UASSERT_UABORTS(uvector_insert_at(v, 500, G_INT(400)));

    uvector_destroy(v);
    */
}

int cmp(const char *s1, const char *s2)
{
    ugeneric_t gv1 = ugeneric_parse(s1);
    ugeneric_t gv2 = ugeneric_parse(s2);
    UASSERT_NO_ERROR(gv1);
    UASSERT_NO_ERROR(gv2);
    uvector_t *v1 = G_AS_PTR(gv1);
    uvector_t *v2 = G_AS_PTR(gv2);
    int diff = uvector_compare(v1, v2, NULL);
    uvector_destroy(v1);
    uvector_destroy(v2);
    return diff;
}

void test_uvector_compare(void)
{
    uvector_t *v1 = uvector_create();
    uvector_t *v2 = uvector_create();
    uvector_append(v1, G_STR(ustring_dup("s1")));
    uvector_append(v2, G_STR(ustring_dup("s1")));
    UASSERT(uvector_compare(v1, v2, NULL) == 0);
    uvector_set_at(v1, 0, G_STR(ustring_dup("s3")));
    UASSERT(uvector_compare(v1, v2, NULL) != 0);
    uvector_set_at(v2, 0, G_STR(ustring_dup("s4")));
    UASSERT(uvector_compare(v1, v2, NULL) < 0);
    uvector_destroy(v1);
    uvector_destroy(v2);

    UASSERT(cmp("[]", "[]") == 0);
    UASSERT(cmp("[1]", "[1]") == 0);
    UASSERT(cmp("[1, 2, 3]", "[1, 2, 3]") == 0);
    UASSERT(cmp("[1, 2]", "[1, 2, 3]") < 0);
    UASSERT(cmp("[1, 2, 3]", "[1, 2]") > 0);
    UASSERT(cmp("[3, 2, 3]", "[1, 2, 3]") > 0);
}

int main(int argc, char **argv)
{
    (void)argv;
    test_uvector_api();
    test_uvector_serialization(argc > 1);
    test_uvector_next_permutation(argc > 1);
    test_uvector_copy(argc > 1);
    test_uvector_bsearch();
    test_uvector_compare();

    return EXIT_SUCCESS;
}
