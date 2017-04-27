#include <stdio.h>
#include <stdlib.h>
#include "vector.h"
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
    ASSERT(n);

    n->name = string_dup(nage->name);
    n->age = nage->age;

    return n;
}

void test_vector_copy(bool verbose)
{
    (void)verbose;

    vector_t *v = vector_create();
    vector_set_destroyer(v, free_nage_t);
    vector_set_copier(v, copy_nage_t);

    nage_t *n = umalloc(sizeof(*n));

    *n = (nage_t){string_dup("john"), 34};
    vector_append(v, G_PTR(n)); // v owns *n data

    vector_t *v2 = vector_copy(v); // v2 has shallow copy of *n2
    vector_t *v3 = vector_deep_copy(v); // v3 has deep copy of *n2, must set own

    vector_destroy(v);
    vector_destroy(v2);
    vector_destroy(v3);
}

void test_vector_bsearch(void)
{
    vector_t *v = vector_create();
    vector_append(v, G_INT(1));
    vector_append(v, G_INT(2));
    vector_append(v, G_INT(3));
    vector_append(v, G_INT(4));

    vector_sort(v); // [1, 2, 3, 4]]
    ASSERT_INT_EQ(vector_bsearch(v, G_INT(1)), 0);
    ASSERT_INT_EQ(vector_bsearch(v, G_INT(2)), 1);
    ASSERT_INT_EQ(vector_bsearch(v, G_INT(3)), 2);
    ASSERT_INT_EQ(vector_bsearch(v, G_INT(4)), 3);
    ASSERT_INT_EQ(vector_bsearch(v, G_INT(400)), SIZE_MAX);
    ASSERT_INT_EQ(vector_bsearch(v, G_INT(-400)), SIZE_MAX);

    vector_remove_at(v, 1); // [1, 3, 4]
    ASSERT_INT_EQ(vector_bsearch(v, G_INT(1)), 0);
    ASSERT_INT_EQ(vector_bsearch(v, G_INT(2)), SIZE_MAX);
    ASSERT_INT_EQ(vector_bsearch(v, G_INT(3)), 1);
    ASSERT_INT_EQ(vector_bsearch(v, G_INT(4)), 2);
    ASSERT_INT_EQ(vector_bsearch(v, G_INT(400)), SIZE_MAX);
    ASSERT_INT_EQ(vector_bsearch(v, G_INT(-400)), SIZE_MAX);

    vector_pop_back(v); // [1, 3]
    ASSERT_INT_EQ(vector_bsearch(v, G_INT(1)), 0);
    ASSERT_INT_EQ(vector_bsearch(v, G_INT(2)), SIZE_MAX);
    ASSERT_INT_EQ(vector_bsearch(v, G_INT(3)), 1);
    ASSERT_INT_EQ(vector_bsearch(v, G_INT(4)), SIZE_MAX);
    ASSERT_INT_EQ(vector_bsearch(v, G_INT(400)), SIZE_MAX);
    ASSERT_INT_EQ(vector_bsearch(v, G_INT(-400)), SIZE_MAX);

    vector_pop_at(v, 0); // [3]
    ASSERT_INT_EQ(vector_bsearch(v, G_INT(1)), SIZE_MAX);
    ASSERT_INT_EQ(vector_bsearch(v, G_INT(2)), SIZE_MAX);
    ASSERT_INT_EQ(vector_bsearch(v, G_INT(3)), 0);
    ASSERT_INT_EQ(vector_bsearch(v, G_INT(4)), SIZE_MAX);
    ASSERT_INT_EQ(vector_bsearch(v, G_INT(400)), SIZE_MAX);
    ASSERT_INT_EQ(vector_bsearch(v, G_INT(-400)), SIZE_MAX);

    vector_pop_at(v, 0);
    ASSERT_INT_EQ(vector_get_size(v), 0);
    ASSERT_INT_EQ(vector_bsearch(v, G_INT(1)), SIZE_MAX);
    ASSERT_INT_EQ(vector_bsearch(v, G_INT(2)), SIZE_MAX);
    ASSERT_INT_EQ(vector_bsearch(v, G_INT(3)), SIZE_MAX);
    ASSERT_INT_EQ(vector_bsearch(v, G_INT(4)), SIZE_MAX);
    ASSERT_INT_EQ(vector_bsearch(v, G_INT(400)), SIZE_MAX);
    ASSERT_INT_EQ(vector_bsearch(v, G_INT(-400)), SIZE_MAX);

    vector_destroy(v);
}

void test_vector_next_permutation(bool verbose)
{
    vector_t *v = vector_create();
    vector_append(v, G_INT(4));
    vector_append(v, G_INT(3));
    vector_append(v, G_INT(1));
    vector_append(v, G_INT(2));

    do {
        if (verbose)
            vector_print(v);
    } while (vector_next_permutation(v));

    vector_destroy(v);
}

void test_vector_serialization(bool verbose)
{
    char *vs;
    vector_t *v = vector_create();

    vs = vector_as_str(v);
    ASSERT_STR_EQ(vs, "[]");
    ufree(vs);

    vector_append(v, G_INT(11));
    vector_append(v, G_INT(22));
    vector_append(v, G_INT(33));
    vector_append(v, G_STR(string_dup("44")));
    vector_append(v, G_CSTR("5"));

    vector_append(v, G_VECTOR(vector_deep_copy(v)));
    vector_append(v, G_VECTOR(vector_deep_copy(v)));

    if (verbose)
        vector_print(v);
    vs = vector_as_str(v);
    ASSERT_STR_EQ(vs,
                  "[11, 22, 33, \"44\", \"5\", [11, 22, 33, \"44\", \"5\"], [11, 22, 33, \"44\", \"5\", [11, 22, 33, \"44\", \"5\"]]]");
    ufree(vs);
    vector_destroy(v);
}

void test_vector_api()
{
    vector_t *v = vector_create();
    vector_append(v, G_INT(0));
    ASSERT(vector_get_capacity(v) >= VECTOR_INITIAL_CAPACITY);
    vector_reserve_capacity(v, 1000);
    ASSERT_INT_EQ(vector_get_capacity(v), 1000);
    vector_destroy(v);

    v = vector_create_with_size(3, G_NULL);
    ASSERT_INT_EQ(vector_get_size(v), 3);

    vector_append(v, G_INT(11));
    vector_append(v, G_INT(22));
    vector_append(v, G_INT(33));
    ASSERT_INT_EQ(vector_get_size(v), 6);
    ASSERT(G_IS_NULL(vector_get_at(v, 0)));
    ASSERT_INT_EQ(G_AS_INT(vector_get_at(v, 3)), 11);
    ASSERT_INT_EQ(G_AS_INT(vector_get_at(v, 4)), 22);
    ASSERT_INT_EQ(G_AS_INT(vector_get_at(v, 5)), 33);

    vector_swap(v, 4, 5);
    ASSERT_INT_EQ(G_AS_INT(vector_get_at(v, 4)), 33);
    ASSERT_INT_EQ(G_AS_INT(vector_get_at(v, 5)), 22);

    vector_resize(v, 0, G_NULL);
    ASSERT(vector_is_empty(v));

    vector_append(v, G_INT(44));
    ASSERT_INT_EQ(vector_get_size(v), 1);
    ASSERT_INT_EQ(G_AS_INT(vector_get_at(v, 0)), 44);

    vector_set_at(v, 0, G_INT(2222));
    ASSERT_INT_EQ(G_AS_INT(vector_get_at(v, 0)), 2222);

    generic_t *a = vector_get_cells(v);
    ASSERT_INT_EQ(G_AS_INT(a[0]), 2222);
    vector_destroy(v);

    v = vector_create();
    ASSERT_INT_EQ(vector_get_size(v), 0);
    ASSERT(vector_is_empty(v));
    vector_append(v, G_INT(11));
    ASSERT(!vector_is_empty(v));
    ASSERT_INT_EQ(vector_get_size(v), 1);
    ASSERT_INT_EQ(G_AS_INT(vector_pop_back(v)), 11);
    ASSERT(vector_is_empty(v));
    ASSERT_INT_EQ(vector_get_size(v), 0);

    vector_resize(v, 0, G_NULL);
    vector_append(v, G_INT(200));
    vector_append(v, G_INT(100));
    ASSERT_INT_EQ(vector_get_size(v), 2);
    ASSERT_INT_EQ(G_AS_INT(vector_get_at(v, 0)), 200);
    ASSERT_INT_EQ(G_AS_INT(vector_get_at(v, 1)), 100);
    vector_insert_at(v, 1, G_INT(300));
    ASSERT_INT_EQ(vector_get_size(v), 3);
    ASSERT_INT_EQ(G_AS_INT(vector_get_at(v, 0)), 200);
    ASSERT_INT_EQ(G_AS_INT(vector_get_at(v, 1)), 300);
    ASSERT_INT_EQ(G_AS_INT(vector_get_at(v, 2)), 100);
    ASSERT_INT_EQ(G_AS_INT(vector_pop_at(v, 0)), 200);
    ASSERT_INT_EQ(vector_get_size(v), 2);
    ASSERT_INT_EQ(G_AS_INT(vector_pop_at(v, 0)), 300);
    ASSERT_INT_EQ(vector_get_size(v), 1);
    ASSERT_INT_EQ(G_AS_INT(vector_pop_at(v, 0)), 100);
    ASSERT_INT_EQ(vector_get_size(v), 0);
    vector_resize(v, 1000, G_NULL);
    ASSERT_INT_EQ(vector_get_size(v), 1000);
    vector_destroy(v);

    v = vector_create();
    vector_append(v, G_STR(string_dup("1")));
    vector_append(v, G_STR(string_dup("2")));
    vector_append(v, G_STR(string_dup("3")));
    vector_append(v, G_STR(string_dup("4")));
    vector_resize(v, 2, G_NULL);
    vector_resize(v, 1, G_NULL);
    vector_clear(v);
    vector_append(v, G_STR(string_dup("5")));
    ASSERT_INT_EQ(vector_get_size(v), 1);
    vector_destroy(v);

    v = vector_create();
    vector_append(v, G_STR(string_dup("string")));
    vector_set_at(v, 0, G_STR(string_dup("string2")));
    vector_destroy(v);

    v = vector_create();
    vector_append(v, G_INT(123));
    ASSERT_INT_EQ(G_AS_INT(vector_get_at(v, 0)), 123);
    ASSERT_INT_EQ(G_AS_INT(vector_get_back(v)), 123);
    vector_t *vcopy = vector_copy(v);
    ASSERT(vector_compare(v, vcopy, NULL) == 0);
    vector_destroy(v);
    vector_destroy(vcopy);

    v = vector_create();
    vector_reserve_capacity(v, 1024);
    vector_shrink_to_size(v);
    vector_destroy(v);

/*
    ASSERT_ABORTS(vector_destroy(v));
    ASSERT_ABORTS(vector_destroy(NULL));
    ASSERT_ABORTS(vector_append(v, G_INT(0)));
    ASSERT_ABORTS(vector_pop_back(v));
    ASSERT_ABORTS(vector_pop_front(v));
    ASSERT_ABORTS(vector_get_cells(v));
    ASSERT_ABORTS(vector_get_size(v));
    ASSERT_ABORTS(vector_resize(v, 100 G_NULL));
    ASSERT_ABORTS(vector_reserve_capacity(v, 100));
    ASSERT_ABORTS(vector_get_capacity(v));
    ASSERT_ABORTS(vector_is_empty(v));
    ASSERT_ABORTS(vector_swap(v, 1, 2));
    ASSERT_ABORTS(vector_insert_at(v, 1, G_INT(100)));
    ASSERT_ABORTS(vector_get_at(v, 1));
    ASSERT_ABORTS(vector_set_at(v, 1, G_INT(0)));

    v = vector_create(0);
    ASSERT_ABORTS(vector_get_at(v, 1));
    ASSERT_ABORTS(vector_set_at(v, 1, G_INT(0)));

    vector_resize(v, 100, G_NULL);
    ASSERT_ABORTS(vector_swap(v, 150, 250));
    ASSERT_ABORTS(vector_swap(v, 50, 250));
    ASSERT_ABORTS(vector_insert_at(v, 500, G_INT(400)));

    vector_destroy(v);
    */
}

int cmp(const char *s1, const char *s2)
{
    generic_t gv1 = generic_parse(s1);
    generic_t gv2 = generic_parse(s2);
    ASSERT_NO_ERROR(gv1);
    ASSERT_NO_ERROR(gv2);
    vector_t *v1 = G_AS_PTR(gv1);
    vector_t *v2 = G_AS_PTR(gv2);
    int diff = vector_compare(v1, v2, NULL);
    vector_destroy(v1);
    vector_destroy(v2);
    return diff;
}

void test_vector_compare(void)
{
    vector_t *v1 = vector_create();
    vector_t *v2 = vector_create();
    vector_append(v1, G_STR(string_dup("s1")));
    vector_append(v2, G_STR(string_dup("s1")));
    ASSERT(vector_compare(v1, v2, NULL) == 0);
    vector_set_at(v1, 0, G_STR(string_dup("s3")));
    ASSERT(vector_compare(v1, v2, NULL) != 0);
    vector_set_at(v2, 0, G_STR(string_dup("s4")));
    ASSERT(vector_compare(v1, v2, NULL) < 0);
    vector_destroy(v1);
    vector_destroy(v2);

    ASSERT(cmp("[]", "[]") == 0);
    ASSERT(cmp("[1]", "[1]") == 0);
    ASSERT(cmp("[1, 2, 3]", "[1, 2, 3]") == 0);
    ASSERT(cmp("[1, 2]", "[1, 2, 3]") < 0);
    ASSERT(cmp("[1, 2, 3]", "[1, 2]") > 0);
    ASSERT(cmp("[3, 2, 3]", "[1, 2, 3]") > 0);
}

int main(int argc, char **argv)
{
    (void)argv;
    test_vector_api();
    test_vector_serialization(argc > 1);
    test_vector_next_permutation(argc > 1);
    test_vector_copy(argc > 1);
    test_vector_bsearch();
    test_vector_compare();

    return EXIT_SUCCESS;
}
