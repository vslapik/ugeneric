#include "bst.h"

#include "file_utils.h"
#include "mem.h"
#include "string_utils.h"
#include "ut_utils.h"
#include "vector.h"

const char *inorder_keys = "[-311, 0, 1, 2, 15, 42, 100, 123, 140, 143, 144, 145, 146, 150, 1000, 2000, 3000, 4000]";
const char *inorder_values = "[-3113, 0, -1, -2, -15, \"-42\", -100, [1, 11, 111, 1111], -140, -143, -144, -145, -146, -150, -1000, -2000, -3.14286, -4004]";
const char *preorder_keys = "[-311, 0, 42, 15, 123, 140, 143, 144, 150, 146, 145, 4000, 3000, 2000, 1000, 100, 2, 1]";
const char *preorder_values = "[-3113, 0, \"-42\", -15, [1, 11, 111, 1111], -140, -143, -144, -150, -146, -145, -4004, -3.14286, -2000, -1000, -100, -2, -1]";
const char *postorder_keys = "[1, 0, -311, 2, 100, 15, 42, 1000, 145, 144, 143, 140, 123, 146, 150, 2000, 3000, 4000]";
const char *postorder_values = "[-1, 0, -3113, -2, -100, -15, \"-42\", -1000, -145, -144, -143, -140, [1, 11, 111, 1111], -146, -150, -2000, -3.14286, -4004]";

bool cb(ugeneric_t k, ugeneric_t v, void *data)
{
    uvector_t *kv = data;
    uvector_t *keys = G_AS_PTR(uvector_get_at(kv, 0));
    uvector_t *values = G_AS_PTR(uvector_get_at(kv, 1));
    uvector_append(keys, k);
    uvector_append(values, v);

    return false;
}

bool _tree_keys_to_str(ugeneric_t k, ugeneric_t v, void *data)
{
    (void)v;
    ubuffer_t *buf = data;
    ugeneric_serialize(k, buf);

    return false;
}

static void _check_relationship(ubst_t *b, ugeneric_t n, ugeneric_t p, ugeneric_t s, ugeneric_t d)
{
    UASSERT_INT_EQ(G_AS_INT(ubst_get_inorder_predecessor(b, n, d)), G_AS_INT(p));
    UASSERT_INT_EQ(G_AS_INT(ubst_get_inorder_successor(b, n, d)), G_AS_INT(s));
}

static ubst_t *_create_test_tree(void)
{
    /*         3
     *      /     \
     *     1       5
     *    / \     / \
     *   0   2   4   6
     */

    ubst_t *b = ubst_create();
    ubst_put(b, G_INT(3), G_INT(3));
    ubst_put(b, G_INT(1), G_INT(1));
    ubst_put(b, G_INT(5), G_INT(5));
    ubst_put(b, G_INT(2), G_INT(2));
    ubst_put(b, G_INT(4), G_INT(4));
    ubst_put(b, G_INT(0), G_INT(0));
    ubst_put(b, G_INT(6), G_INT(6));

    return b;
}

static ubst_t *_create_test_tree2(void)
{
    /*       2
     *      /
     *     1
     *    /
     *   0
     */

    ubst_t *b = ubst_create();
    ubst_put(b, G_INT(2), G_INT(2));
    ubst_put(b, G_INT(1), G_INT(1));
    ubst_put(b, G_INT(0), G_INT(0));

    return b;
}

static ubst_t *_create_test_tree3(void)
{
    /*       0
     *        \
     *         1
     *          \
     *           2
     */

    ubst_t *b = ubst_create();
    ubst_put(b, G_INT(0), G_INT(0));
    ubst_put(b, G_INT(1), G_INT(1));
    ubst_put(b, G_INT(2), G_INT(2));

    return b;
}

void test_api(void)
{
    ubst_t *b = _create_test_tree();

    UASSERT_INT_EQ(G_AS_INT(ubst_get_min(b)), 0);
    UASSERT_INT_EQ(G_AS_INT(ubst_get_max(b)), 6);
    _check_relationship(b, G_INT(0), G_INT(-1), G_INT(1), G_INT(-1));
    _check_relationship(b, G_INT(1), G_INT(0), G_INT(2), G_INT(-1));
    _check_relationship(b, G_INT(2), G_INT(1), G_INT(3), G_INT(-1));
    _check_relationship(b, G_INT(3), G_INT(2), G_INT(4), G_INT(-1));
    _check_relationship(b, G_INT(4), G_INT(3), G_INT(5), G_INT(-1));
    _check_relationship(b, G_INT(5), G_INT(4), G_INT(6), G_INT(-1));
    _check_relationship(b, G_INT(6), G_INT(5), G_INT(-1), G_INT(-1));
    ubst_destroy(b);

    b = _create_test_tree2();
    UASSERT_INT_EQ(G_AS_INT(ubst_get_min(b)), 0);
    UASSERT_INT_EQ(G_AS_INT(ubst_get_max(b)), 2);
    _check_relationship(b, G_INT(0), G_INT(-1), G_INT(1), G_INT(-1));
    _check_relationship(b, G_INT(1), G_INT(0), G_INT(2), G_INT(-1));
    _check_relationship(b, G_INT(2), G_INT(1), G_INT(-1), G_INT(-1));
    ubst_destroy(b);

    b = _create_test_tree3();
    UASSERT_INT_EQ(G_AS_INT(ubst_get_min(b)), 0);
    UASSERT_INT_EQ(G_AS_INT(ubst_get_max(b)), 2);
    _check_relationship(b, G_INT(0), G_INT(-1), G_INT(1), G_INT(-1));
    _check_relationship(b, G_INT(1), G_INT(0), G_INT(2), G_INT(-1));
    _check_relationship(b, G_INT(2), G_INT(1), G_INT(-1), G_INT(-1));
    ubst_destroy(b);
}

void test_pop(void)
{
    ubuffer_t buf = {0};
    ubst_t *b;

    b = _create_test_tree();
    ubst_pop(b, G_INT(2), G_INT(-1));
    ubst_pop(b, G_INT(4), G_INT(-1));
    ubst_traverse(b, UBST_INORDER, _tree_keys_to_str, &buf);
    ubuffer_null_terminate(&buf);
    UASSERT_STR_EQ(buf.data, "01356");
    ubuffer_reset(&buf);
    ubst_destroy(b);

    b = _create_test_tree();
    ubst_pop(b, G_INT(1), G_INT(-1));
    ubst_pop(b, G_INT(5), G_INT(-1));
    ubst_traverse(b, UBST_INORDER, _tree_keys_to_str, &buf);
    ubuffer_null_terminate(&buf);
    UASSERT_STR_EQ(buf.data, "02346");
    ubuffer_reset(&buf);
    ubst_destroy(b);

    b = _create_test_tree();
    ubst_pop(b, G_INT(3), G_INT(-1));
    ubst_traverse(b, UBST_INORDER, _tree_keys_to_str, &buf);
    ubuffer_null_terminate(&buf);
    UASSERT_STR_EQ(buf.data, "012456");
    ubuffer_reset(&buf);
//    ubst_dump_to_dot(b, "UBST2", false, stdout);
    ubst_destroy(b);

    ufree(buf.data);
}

bool _find_2(ugeneric_t k, ugeneric_t v, void *data)
{
    (void)v;
    uvector_t *keys = data;
    uvector_append(keys, k);

    // Stop when 2 is found.
    return (G_AS_INT(k) == 2) ? true : false;
}
void test_traverse(void)
{
    char *str;

    ubst_t *b = ubst_create();
    ubst_put(b, G_INT(1), G_INT(-1));
    ubst_put(b, G_INT(2), G_INT(-2));
    ubst_put(b, G_INT(100), G_INT(-100));
    ubst_put(b, G_INT(15), G_INT(-15));
    ubst_put(b, G_INT(0), G_INT(0));
    ubst_put(b, G_INT(1000), G_INT(-1000));
    ubst_put(b, G_INT(2000), G_INT(-2000));
    ubst_put(b, G_INT(3000), G_REAL(-22.0/7));
    ubst_put(b, G_INT(4000), G_INT(-4004));
    ubst_put(b, G_INT(-311), G_INT(-3113));
    ubst_put(b, G_INT(145), G_INT(-145));
    ubst_put(b, G_INT(144), G_INT(-144));
    ubst_put(b, G_INT(143), G_INT(-143));
    ubst_put(b, G_INT(146), G_INT(-146));
    ubst_put(b, G_INT(150), G_INT(-150));
    ubst_put(b, G_INT(140), G_INT(-140));
    ubst_put(b, G_INT(42), G_STR(ustring_dup("-42")));
    //ubst_dump_to_dot(b, "UBST", false, stdout);

    UASSERT_INT_EQ(G_AS_INT(ubst_get_min(b)), -3113);
    UASSERT_INT_EQ(G_AS_INT(ubst_get_max(b)), -4004);

    uvector_t *v = uvector_create();
    uvector_append(v, G_INT(1));
    uvector_append(v, G_INT(11));
    uvector_append(v, G_INT(111));
    uvector_append(v, G_INT(1111));
    ubst_put(b, G_INT(123), G_VECTOR(v));

    uvector_t *kv = uvector_create();
    uvector_t *keys = uvector_create();
    uvector_t *values = uvector_create();
    uvector_drop_data_ownership(values);
    uvector_drop_data_ownership(keys);
    uvector_append(kv, G_VECTOR(keys));
    uvector_append(kv, G_VECTOR(values));

    ubst_traverse(b, UBST_INORDER, cb, kv);
    str = uvector_as_str(keys); UASSERT_STR_EQ(str, inorder_keys); ufree(str);
    str = uvector_as_str(values); UASSERT_STR_EQ(str, inorder_values); ufree(str);
    uvector_clear(keys);
    uvector_clear(values);

    ubst_traverse(b, UBST_POSTORDER, cb, kv);
    str = uvector_as_str(keys); UASSERT_STR_EQ(str, preorder_keys); ufree(str);
    str = uvector_as_str(values); UASSERT_STR_EQ(str, preorder_values); ufree(str);
    uvector_clear(keys);
    uvector_clear(values);

    ubst_traverse(b, UBST_PREORDER, cb, kv);
    str = uvector_as_str(keys); UASSERT_STR_EQ(str, postorder_keys); ufree(str);
    str = uvector_as_str(values); UASSERT_STR_EQ(str, postorder_values); ufree(str);
    uvector_clear(keys);
    uvector_clear(values);

    uvector_destroy(kv);
    ubst_destroy(b);

    // Test iteration stop
    b = _create_test_tree();
    uvector_t *k = uvector_create();
    ubst_traverse(b, UBST_POSTORDER, _find_2, k);
    str = uvector_as_str(k);
    UASSERT_STR_EQ(str, "[0, 2]");
    uvector_clear(k);
    ufree(str);

    ubst_traverse(b, UBST_PREORDER, _find_2, k);
    str = uvector_as_str(k);
    UASSERT_STR_EQ(str, "[3, 1, 0, 2]");
    uvector_clear(k);
    ufree(str);

    ubst_traverse(b, UBST_INORDER, _find_2, k);
    str = uvector_as_str(k);
    UASSERT_STR_EQ(str, "[0, 1, 2]");
    uvector_clear(k);
    ufree(str);

    uvector_destroy(k);
    ubst_destroy(b);
}

void test_large_bst(void)
{
    ubst_t *b = ubst_create_ext(UBST_RB_BALANCING);
    const char *path = "utdata/dict_data.txt";
    ugeneric_t g = ufile_read_lines(path);
    UASSERT_NO_ERROR(g);
    uvector_t *v = G_AS_PTR(g);

    size_t vsize = uvector_get_size(v);
    for (size_t i = 0; i < vsize; i++)
    {
       uvector_t *v2 = ustring_split(G_AS_STR(uvector_get_at(v, i)), " ");
       uvector_drop_data_ownership(v2);
       ubst_put(b, uvector_get_at(v2, 0), uvector_get_at(v2, 1));
       uvector_destroy(v2);
    }
    uvector_destroy(v);
    UASSERT_INT_EQ(500, ubst_get_size(b));

    //ubst_dump_to_dot(b, "large_bst", false, stdout);
    uvector_destroy(items);
    ubst_destroy(b);

    size_t N = 100;
    b = ubst_create_ext(UBST_NO_BALANCING);
    for (size_t i = N; i > 1; i--)
    {
       ubst_put(b, G_INT(i), G_INT(i));
    }
    //ubst_dump_to_dot(b, "large_bst", false, stdout);
    ubst_destroy(b);

    b = ubst_create_ext(UBST_RB_BALANCING);
    for (size_t i = 0; i < N; i++)
    {
       ubst_put(b, G_INT(i), G_INT(i));
    }
    //ubst_dump_to_dot(b, "large_bst", false, stdout);

    ubst_destroy(b);

    b = ubst_create_ext(UBST_RB_BALANCING);
    for (size_t i = 0; i < N; i++)
    {
       ubst_put(b, G_INT(N - i), G_INT(N - i));
       ubst_put(b, G_INT(i), G_INT(i));
    }
    //ubst_dump_to_dot(b, "large_bst", false, stdout);

    ubst_destroy(b);
}

void test_ubst_iterator(void)
{
    ubst_t *b = _create_test_tree();
    ubst_iterator_t *bi = ubst_iterator_create(b);
    int t = 2;

    while (t--)
    {
        UASSERT_INT_EQ(G_AS_INT(ubst_iterator_get_next(bi).v), 0);
        UASSERT_INT_EQ(G_AS_INT(ubst_iterator_get_next(bi).v), 1);
        UASSERT_INT_EQ(G_AS_INT(ubst_iterator_get_next(bi).v), 2);
        UASSERT_INT_EQ(G_AS_INT(ubst_iterator_get_next(bi).v), 3);
        UASSERT_INT_EQ(G_AS_INT(ubst_iterator_get_next(bi).v), 4);
        UASSERT_INT_EQ(G_AS_INT(ubst_iterator_get_next(bi).v), 5);
        UASSERT_INT_EQ(G_AS_INT(ubst_iterator_get_next(bi).v), 6);
        ubst_iterator_reset(bi);
    }

    ubst_iterator_destroy(bi);
    ubst_destroy(b);

    //
    ubst_t *b2 = ubst_create();
    ubst_put(b2, G_INT(6), G_STR(ustring_dup("6")));
    ubst_put(b2, G_INT(5), G_STR(ustring_dup("5")));
    ubst_put(b2, G_INT(4), G_STR(ustring_dup("4")));
    ubst_put(b2, G_INT(3), G_STR(ustring_dup("3")));
    ubst_put(b2, G_INT(2), G_STR(ustring_dup("2")));
    ubst_put(b2, G_INT(1), G_STR(ustring_dup("1")));
    ubst_put(b2, G_INT(0), G_STR(ustring_dup("0")));
    ubst_iterator_t *bi2 = ubst_iterator_create(b2);

    UASSERT_STR_EQ(G_AS_STR(ubst_iterator_get_next(bi2).v), "0");
    UASSERT_STR_EQ(G_AS_STR(ubst_iterator_get_next(bi2).v), "1");
    UASSERT_STR_EQ(G_AS_STR(ubst_iterator_get_next(bi2).v), "2");

    ubst_iterator_destroy(bi2);
    ubst_destroy(b2);
}

void test_rb_put(void)
{
    ubst_t *b = ubst_create();
    ubst_put(b, G_INT(8), G_INT(8));
    ubst_put(b, G_INT(1), G_INT(1));
    ubst_put(b, G_INT(3), G_INT(3));

    ubst_put(b, G_INT(5), G_INT(5));
    ubst_put(b, G_INT(6), G_INT(6));
    ubst_put(b, G_INT(7), G_INT(7));
    ubst_put(b, G_INT(9), G_INT(9));
    ubst_put(b, G_INT(10), G_INT(10));
    ubst_put(b, G_INT(11), G_INT(11));
    ubst_put(b, G_INT(12), G_INT(12));
    ubst_put(b, G_INT(13), G_INT(13));
    ubst_put(b, G_INT(14), G_INT(14));
    ubst_put(b, G_INT(-1), G_INT(-1));
    ubst_put(b, G_INT(4), G_INT(4));

    // update
    ubst_put(b, G_INT(6), G_INT(777));
    ubst_put(b, G_INT(14), G_INT(777));
    ubst_put(b, G_INT(5), G_INT(555));

    // insert again
    ubst_put(b, G_INT(-1000), G_INT(-1000));
    ubst_put(b, G_INT(-500), G_INT(-500));

    ubst_destroy(b);
}

void test_bst_cmp(void)
{
    ubst_t *b1 = _create_test_tree();
    ubst_t *b2 = _create_test_tree();
    UASSERT(ubst_compare(b1, b2, NULL) == 0);

    ubst_put(b1, G_INT(3), G_INT(33));
    UASSERT(ubst_compare(b1, b2, NULL) > 0);

    ubst_put(b2, G_INT(3), G_INT(333));
    UASSERT(ubst_compare(b1, b2, NULL) < 0);

    ubst_put(b1, G_INT(3), G_INT(333));
    UASSERT(ubst_compare(b1, b2, NULL) == 0);

    ubst_destroy(b1);
    ubst_destroy(b2);
}

/*
 * test_bst [rb|splay|nb]
 *
*/
int main(int argc, char **argv)
{
    if (argc == 2)
    {
        if (!strcmp(argv[1], "rb"))
        {
            ubst_set_default_balancing_mode(UBST_RB_BALANCING);
        }
        else if (!strcmp(argv[1], "splay"))
        {
            ubst_set_default_balancing_mode(UBST_SPLAY_BALANCING);
        }
        else if (!strcmp(argv[1], "nb"))
        {
            ubst_set_default_balancing_mode(UBST_NO_BALANCING);
        }
        else
        {
            goto cli_parse_error;
        }
    }
    else if (argc > 2)
    {
        goto cli_parse_error;
    }

    test_ubst_iterator();
    test_traverse();
    test_api();
    test_pop();
    test_large_bst();

    test_bst_cmp();

    return 0;

cli_parse_error:
    fprintf(stderr, "Can't parse provided command line");
    return EXIT_FAILURE;
}
