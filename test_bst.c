#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "ut_utils.h"
#include "mem.h"
#include "bst.h"
#include "string_utils.h"
#include "file_utils.h"
#include "vector.h"

const char *inorder_keys = "[-311, 0, 1, 2, 15, 42, 100, 123, 140, 143, 144, 145, 146, 150, 1000, 2000, 3000, 4000]";
const char *inorder_values = "[-3113, 0, -1, -2, -15, \"-42\", -100, [1, 11, 111, 1111], -140, -143, -144, -145, -146, -150, -1000, -2000, -3.14286, -4004]";
const char *preorder_keys = "[-311, 0, 42, 15, 123, 140, 143, 144, 150, 146, 145, 4000, 3000, 2000, 1000, 100, 2, 1]";
const char *preorder_values = "[-3113, 0, \"-42\", -15, [1, 11, 111, 1111], -140, -143, -144, -150, -146, -145, -4004, -3.14286, -2000, -1000, -100, -2, -1]";
const char *postorder_keys = "[1, 0, -311, 2, 100, 15, 42, 1000, 145, 144, 143, 140, 123, 146, 150, 2000, 3000, 4000]";
const char *postorder_values = "[-1, 0, -3113, -2, -100, -15, \"-42\", -1000, -145, -144, -143, -140, [1, 11, 111, 1111], -146, -150, -2000, -3.14286, -4004]";

bool cb(generic_t k, generic_t v, void *data)
{
    vector_t *kv = data;
    vector_t *keys = G_AS_PTR(vector_get_at(kv, 0));
    vector_t *values = G_AS_PTR(vector_get_at(kv, 1));
    vector_append(keys, k);
    vector_append(values, v);

    return false;
}

bool _tree_keys_to_str(generic_t k, generic_t v, void *data)
{
    (void)v;
    buffer_t *buf = data;
    generic_serialize(k, buf);

    return false;
}

static void _check_relationship(bst_t *b, generic_t n, generic_t p, generic_t s, generic_t d)
{
    ASSERT_INT_EQ(G_AS_INT(bst_get_inorder_predecessor(b, n, d)), G_AS_INT(p));
    ASSERT_INT_EQ(G_AS_INT(bst_get_inorder_successor(b, n, d)), G_AS_INT(s));
}

static bst_t *_create_test_tree(void)
{
    /*         3
     *      /     \
     *     1       5
     *      \     /
     *       2   4
     */

    bst_t *b = bst_create();
    bst_put(b, G_INT(3), G_INT(3));
    bst_put(b, G_INT(1), G_INT(1));
    bst_put(b, G_INT(5), G_INT(5));
    bst_put(b, G_INT(2), G_INT(2));
    bst_put(b, G_INT(4), G_INT(4));

    return b;
}

void test_api(void)
{
    bst_t *b = _create_test_tree();

    ASSERT_INT_EQ(G_AS_INT(bst_get_min(b)), 1);
    ASSERT_INT_EQ(G_AS_INT(bst_get_max(b)), 5);

    _check_relationship(b, G_INT(1), G_INT(0), G_INT(2), G_INT(0));
    _check_relationship(b, G_INT(2), G_INT(1), G_INT(3), G_INT(0));
    _check_relationship(b, G_INT(3), G_INT(2), G_INT(4), G_INT(0));
    _check_relationship(b, G_INT(4), G_INT(3), G_INT(5), G_INT(0));
    _check_relationship(b, G_INT(5), G_INT(4), G_INT(0), G_INT(0));

    bst_destroy(b);
}

void test_pop(void)
{
    buffer_t buf = {0};
    bst_t *b;

    b = _create_test_tree();
    bst_pop(b, G_INT(2), G_INT(-1));
    bst_pop(b, G_INT(4), G_INT(-1));
    bst_traverse(b, BST_INORDER, _tree_keys_to_str, &buf);
    buffer_null_terminate(&buf);
    ASSERT_STR_EQ(buf.data, "135");
    buffer_reset(&buf);
    bst_destroy(b);

    b = _create_test_tree();
    bst_pop(b, G_INT(1), G_INT(-1));
    bst_pop(b, G_INT(5), G_INT(-1));
    bst_traverse(b, BST_INORDER, _tree_keys_to_str, &buf);
    buffer_null_terminate(&buf);
    ASSERT_STR_EQ(buf.data, "234");
    buffer_reset(&buf);
    bst_destroy(b);

    b = _create_test_tree();
    bst_pop(b, G_INT(3), G_INT(-1));
    bst_traverse(b, BST_INORDER, _tree_keys_to_str, &buf);
    buffer_null_terminate(&buf);
    ASSERT_STR_EQ(buf.data, "1245");
    buffer_reset(&buf);
//    bst_dump_to_dot(b, "BST2", false, stdout);
    bst_destroy(b);

    ufree(buf.data);
}

void test_traverse(void)
{
    bst_t *b = bst_create();
    bst_put(b, G_INT(1), G_INT(-1));
    bst_put(b, G_INT(2), G_INT(-2));
    bst_put(b, G_INT(100), G_INT(-100));
    bst_put(b, G_INT(15), G_INT(-15));
    bst_put(b, G_INT(0), G_INT(0));
    bst_put(b, G_INT(1000), G_INT(-1000));
    bst_put(b, G_INT(2000), G_INT(-2000));
    bst_put(b, G_INT(3000), G_REAL(-22.0/7));
    bst_put(b, G_INT(4000), G_INT(-4004));
    bst_put(b, G_INT(-311), G_INT(-3113));
    bst_put(b, G_INT(145), G_INT(-145));
    bst_put(b, G_INT(144), G_INT(-144));
    bst_put(b, G_INT(143), G_INT(-143));
    bst_put(b, G_INT(146), G_INT(-146));
    bst_put(b, G_INT(150), G_INT(-150));
    bst_put(b, G_INT(140), G_INT(-140));
    bst_put(b, G_INT(42), G_STR(string_dup("-42")));
    //bst_dump_to_dot(b, "BST", false, stdout);

    ASSERT_INT_EQ(G_AS_INT(bst_get_min(b)), -3113);
    ASSERT_INT_EQ(G_AS_INT(bst_get_max(b)), -4004);

    vector_t *v = vector_create_empty();
    vector_append(v, G_INT(1));
    vector_append(v, G_INT(11));
    vector_append(v, G_INT(111));
    vector_append(v, G_INT(1111));
    bst_put(b, G_INT(123), G_VECTOR(v));

    vector_t *kv = vector_create_empty();
    vector_t *keys = vector_create_empty();
    vector_t *values = vector_create_empty();
    vector_drop_data_ownership(values);
    vector_drop_data_ownership(keys);
    vector_append(kv, G_VECTOR(keys));
    vector_append(kv, G_VECTOR(values));

    char *str;
    bst_traverse(b, BST_INORDER, cb, kv);
    str = vector_as_str(keys); ASSERT_STR_EQ(str, inorder_keys); ufree(str);
    str = vector_as_str(values); ASSERT_STR_EQ(str, inorder_values); ufree(str);
    vector_clear(keys);
    vector_clear(values);

    bst_traverse(b, BST_POSTORDER, cb, kv);
    str = vector_as_str(keys); ASSERT_STR_EQ(str, preorder_keys); ufree(str);
    str = vector_as_str(values); ASSERT_STR_EQ(str, preorder_values); ufree(str);
    vector_clear(keys);
    vector_clear(values);

    bst_traverse(b, BST_PREORDER, cb, kv);
    str = vector_as_str(keys); ASSERT_STR_EQ(str, postorder_keys); ufree(str);
    str = vector_as_str(values); ASSERT_STR_EQ(str, postorder_values); ufree(str);
    vector_clear(keys);
    vector_clear(values);

    //bst_dump_to_dot(b, "BST", false, stdout);
    //bst_pop(b, G_INT(123));

    /*
    bst_dump_to_dot(b, "BST2", false, stdout);
    bst_delete(b, G_INT(0));
    bst_dump_to_dot(b, "BST3", false, stdout);
    bst_delete(b, G_INT(2));
    bst_dump_to_dot(b, "BST4", false, stdout);
    */

    vector_destroy(kv);
    bst_destroy(b);
}

void test_large_bst(void)
{
    bst_t *b = bst_create_ext(BST_RB_BALANCING);
    const char *path = "utdata/dict_data.txt";
    generic_t g = file_read_lines(path);
    ASSERT_NO_ERROR(g);
    vector_t *v = G_AS_PTR(g);

    size_t vsize = vector_get_size(v);
    for (size_t i = 0; i < vsize; i++)
    {
       vector_t *v2 = string_split(G_AS_STR(vector_get_at(v, i)), " ");
       vector_drop_data_ownership(v2);
       bst_put(b, vector_get_at(v2, 0), vector_get_at(v2, 1));
       vector_destroy(v2);
    }
    vector_destroy(v);
    //generic_destroy(bst_pop(b, G_STR("PQr"), G_STR("")), ufree);
    //bst_dump_to_dot(b, "large_bst", false, stdout);
    ASSERT_INT_EQ(500, bst_get_size(b));
    bst_destroy(b);

    size_t N = 100;
    b = bst_create_ext(BST_NO_BALANCING);
    for (size_t i = N; i > 1; i--)
    {
       bst_put(b, G_INT(i), G_INT(i));
    }
    //bst_dump_to_dot(b, "large_bst", false, stdout);
    bst_destroy(b);

    b = bst_create_ext(BST_RB_BALANCING);
    for (size_t i = 0; i < N; i++)
    {
       bst_put(b, G_INT(i), G_INT(i));
    }
    //bst_dump_to_dot(b, "large_bst", false, stdout);
    bst_destroy(b);

    b = bst_create_ext(BST_RB_BALANCING);
    for (size_t i = 0; i < N; i++)
    {
       bst_put(b, G_INT(N - i), G_INT(N - i));
       bst_put(b, G_INT(i), G_INT(i));
    }
    //bst_dump_to_dot(b, "large_bst", false, stdout);
    bst_destroy(b);
}

void test_bst_iterator(void)
{
    bst_t *b = _create_test_tree();
    bst_iterator_t *bi = bst_iterator_create(b);
    int t = 2;

    while (t--)
    {
        ASSERT_INT_EQ(G_AS_INT(bst_iterator_get_next(bi).v), 1);
        ASSERT_INT_EQ(G_AS_INT(bst_iterator_get_next(bi).v), 2);
        ASSERT_INT_EQ(G_AS_INT(bst_iterator_get_next(bi).v), 3);
        ASSERT_INT_EQ(G_AS_INT(bst_iterator_get_next(bi).v), 4);
        ASSERT_INT_EQ(G_AS_INT(bst_iterator_get_next(bi).v), 5);
        bst_iterator_reset(bi);
    }

    bst_iterator_destroy(bi);
    bst_destroy(b);
}

void test_rb_put(void)
{

    bst_t *b = bst_create();
    bst_put(b, G_INT(8), G_INT(8));
    bst_put(b, G_INT(1), G_INT(1));
    bst_put(b, G_INT(3), G_INT(3));

    bst_put(b, G_INT(5), G_INT(5));
    bst_put(b, G_INT(6), G_INT(6));
    bst_put(b, G_INT(7), G_INT(7));
    bst_put(b, G_INT(9), G_INT(9));
    bst_put(b, G_INT(10), G_INT(10));
    bst_put(b, G_INT(11), G_INT(11));
    bst_put(b, G_INT(12), G_INT(12));
    bst_put(b, G_INT(13), G_INT(13));
    bst_put(b, G_INT(14), G_INT(14));
    bst_put(b, G_INT(-1), G_INT(-1));
    bst_put(b, G_INT(4), G_INT(4));

    // update
    bst_put(b, G_INT(6), G_INT(777));
    bst_put(b, G_INT(14), G_INT(777));
    bst_put(b, G_INT(5), G_INT(555));

    // insert again
    bst_put(b, G_INT(-1000), G_INT(-1000));
    bst_put(b, G_INT(-500), G_INT(-500));

    bst_destroy(b);

}

void test_rotate(void)
{
    bst_t *b = bst_create();

    bst_put(b, G_INT(5), G_INT(5));
    bst_put(b, G_INT(3), G_INT(3));
    bst_put(b, G_INT(2), G_INT(2));
    bst_put(b, G_INT(4), G_INT(4));
    bst_put(b, G_INT(6), G_INT(6));
    rotate_right(b);
//    bst_dump_to_dot(b, "bst", false, stdout);
    /*
    rotate_right(b);
    rotate_right(b);
    rotate_right(b);
    rotate_right(b);
    rotate_left(b);
    //rotate_right(b);
    bst_dump_to_dot(b, "bst", false, stdout);
    //

*/
    bst_destroy(b);
}

/*
 * test_bst [rb|splay]
 *
*/
int main(int argc, char **argv)
{

    if (argc == 2)
    {
        if (!strcmp(argv[1], "rb"))
        {
            bst_set_default_balancing_mode(BST_RB_BALANCING);
        }
        else if (!strcmp(argv[1], "splay"))
        {
            bst_set_default_balancing_mode(BST_SPLAY_BALANCING);
        }
        else if (!strcmp(argv[1], "nb"))
        {
            bst_set_default_balancing_mode(BST_NO_BALANCING);
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

    //test_rotate();

    test_bst_iterator();
    test_traverse();
    test_api();
    test_pop();
    test_large_bst();

    return 0;

cli_parse_error:
    fprintf(stderr, "Can't parse provided command line");
    return EXIT_FAILURE;
}
