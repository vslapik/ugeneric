#include "struct.h"
#include "ut_utils.h"

void test_struct_convertor(void)
{
    typedef struct {
        long int i1;
        char *s1;
        bool b1;
        struct {
            size_t len;
            bool *cells;
        } booleans;
        struct {
            size_t len;
            char **cells;
        } strings;
        struct {
            size_t len;
            long int *cells;
        } integers;
    } s1_t;

    typedef struct {
        long int i;
        long int j;
        long int k;
    } s2_t;

    typedef struct {
        char *str;
        s1_t *ref;
        struct {
            size_t len;
            s2_t **cells;
        } refs;
    } s3_t;

    const char *input = "{'i1': 1234, 's1': 'a string', 'b1': false, 'strings': ['str1', 'str2', 'str3'], 'integers': [11, 33]}";
    const char *input1 = "{'str': 'fold', 'ref': {}, 'refs': [{'i': 1, 'j': 2, 'k': 3}, {'i': 4, 'j': 5, 'k': 6}, {}]}";

    ugeneric_t g = ugeneric_parse(input);
    UASSERT_NO_ERROR(g);
    ugeneric_t g1 = ugeneric_parse(input1);
    UASSERT_NO_ERROR(g1);

    ugeneric_t empty = G_VECTOR(uvector_create());
    ustruct_data_descriptor_t sdd1[] = {
        {"i1",       G_INT_T,  offsetof(s1_t, i1),       G_INT(-11)},
        {"s1",       G_STR_T,  offsetof(s1_t, s1),       G_STR("default")},
        {"b1",       G_BOOL_T, offsetof(s1_t, b1),       G_BOOL(false)},
        {"booleans", G_BOOL_T, offsetof(s1_t, booleans), empty, true},
        {"strings",  G_STR_T,  offsetof(s1_t, strings),  empty, true},
        {"integers", G_INT_T,  offsetof(s1_t, integers), empty, true},
        {0},
     };
    ustruct_data_descriptor_t sdd2[] = {
        {"i",        G_INT_T, offsetof(s2_t, i),       G_INT(-1)},
        {"j",        G_INT_T, offsetof(s2_t, j),       G_INT(-11)},
        {"k",        G_INT_T, offsetof(s2_t, k),       G_INT(-111)},
        {0},
    };
    ustruct_data_descriptor_t sdd3[] = {
        {"str",      G_STR_T,  offsetof(s3_t, str),     G_STR("str")},
        {"ref",      G_DICT_T, offsetof(s3_t, ref),     G_PTR(0), false, sdd1, sizeof(s1_t)},
        {"refs",     G_DICT_T, offsetof(s3_t, refs),    G_PTR(0), true,  sdd2, sizeof(s2_t)},
        {0},
    };

    // custom_struct_t
    ugeneric_t gptr = ustruct_create_from_dict(G_AS_PTR(g), sizeof(s1_t), sdd1);
    UASSERT_NO_ERROR(gptr);

    s1_t *t = G_AS_PTR(gptr);
    UASSERT_INT_EQ(t->i1, 1234);
    UASSERT_STR_EQ(t->s1, "a string");
    UASSERT(!t->b1);

    UASSERT_INT_EQ(t->booleans.len, 0);
    UASSERT(t->booleans.cells == NULL);

    UASSERT_INT_EQ(t->strings.len, 3);
    UASSERT(t->strings.cells != NULL);
    UASSERT_STR_EQ(t->strings.cells[0], "str1");
    UASSERT_STR_EQ(t->strings.cells[1], "str2");
    UASSERT_STR_EQ(t->strings.cells[2], "str3");

    UASSERT_INT_EQ(t->integers.len, 2);
    UASSERT(t->integers.cells != NULL);
    UASSERT_INT_EQ(t->integers.cells[0], 11);
    UASSERT_INT_EQ(t->integers.cells[1], 33);

    ufree(t->s1);
    ufree(t->booleans.cells);
    ufree(t->strings.cells[0]);
    ufree(t->strings.cells[1]);
    ufree(t->strings.cells[2]);
    ufree(t->strings.cells);
    ufree(t->integers.cells);
    ufree(t);

    // once again but free by data descriptor
    ugeneric_t _ = ustruct_create_from_dict(G_AS_PTR(g), sizeof(s1_t), sdd1);
    UASSERT_NO_ERROR(_);
    ustruct_destroy_by_descriptor(G_AS_PTR(_), sdd1);

    // custom_struct1_t
    gptr = ustruct_create_from_dict(G_AS_PTR(g1), sizeof(s3_t), sdd3);
    UASSERT_NO_ERROR(gptr);
    s3_t *t3 = G_AS_PTR(gptr);
    UASSERT_STR_EQ(t3->str, "fold");
    UASSERT(t3->ref != NULL);
    UASSERT_INT_EQ(t3->ref->i1, -11);
    UASSERT_STR_EQ(t3->ref->s1, "default");
    UASSERT(!t3->ref->b1);
    UASSERT(t3->ref->booleans.cells == NULL);
    UASSERT(t3->ref->strings.cells == NULL);
    UASSERT(t3->ref->integers.cells == NULL);
    UASSERT_INT_EQ(t3->refs.len, 3);
    UASSERT(t3->refs.cells != NULL);
    /*
    printf("%ld %ld %ld\n", t3->refs.cells[0]->i, t3->refs.cells[0]->j, t3->refs.cells[0]->k);
    printf("%ld %ld %ld\n", t3->refs.cells[1]->i, t3->refs.cells[1]->j, t3->refs.cells[1]->k);
    printf("%ld %ld %ld\n", t3->refs.cells[2]->i, t3->refs.cells[2]->j, t3->refs.cells[2]->k);
    */

    ufree(t3->ref->s1);
    ufree(t3->ref);
    ufree(t3->str);
    ufree(t3->refs.cells[0]);
    ufree(t3->refs.cells[1]);
    ufree(t3->refs.cells[2]);
    ufree(t3->refs.cells);
    ufree(t3);

    // once again but free by data descriptor
    ugeneric_t __ = ustruct_create_from_dict(G_AS_PTR(g1), sizeof(s3_t), sdd3);
    UASSERT_NO_ERROR(__);
    ustruct_destroy_by_descriptor(G_AS_PTR(__), sdd3);

    ugeneric_destroy(empty, NULL);
    ugeneric_destroy(g, NULL);
    ugeneric_destroy(g1, NULL);
}


int main(void)
{
    test_struct_convertor();
    return EXIT_SUCCESS;
}
