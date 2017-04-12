#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mem.h"
#include "list.h"
#include "vector.h"
#include "ut_utils.h"

void test_list_api(void)
{
    list_t *l = list_create();
    ASSERT(list_is_empty(l));

    list_append(l, G_INT(1));
    ASSERT(!list_is_empty(l));
    list_append(l, G_INT(2));
    list_append(l, G_INT(3));
    list_append(l, G_INT(4));
    ASSERT(list_get_size(l) == 4);

    ASSERT_INT_EQ(G_AS_INT(list_pop_back(l)), 4);
    ASSERT_INT_EQ(G_AS_INT(list_pop_back(l)), 3);
    ASSERT_INT_EQ(G_AS_INT(list_pop_back(l)), 2);
    ASSERT_INT_EQ(G_AS_INT(list_pop_back(l)), 1);
    ASSERT(list_is_empty(l));
    ASSERT(list_get_size(l) == 0);

    list_clear(l);
    ASSERT(list_is_empty(l));
    ASSERT(list_get_size(l) == 0);

    list_prepend(l, G_INT(11));
    list_prepend(l, G_INT(22));
    ASSERT(list_get_size(l) == 2);
    ASSERT_INT_EQ(G_AS_INT(list_pop_front(l)), 22);
    ASSERT_INT_EQ(G_AS_INT(list_pop_front(l)), 11);

    list_prepend(l, G_INT(100));
    list_append(l, G_INT(200));
    ASSERT_INT_EQ(G_AS_INT(list_get_at(l, 0)), 100);
    ASSERT_INT_EQ(G_AS_INT(list_get_at(l, 1)), 200);

    list_set_at(l, 0, G_INT(500));
    list_set_at(l, 1, G_INT(600));
    ASSERT_INT_EQ(G_AS_INT(list_get_at(l, 0)), 500);
    ASSERT_INT_EQ(G_AS_INT(list_get_at(l, 1)), 600);

    list_insert_at(l, 0, G_INT(700));
    ASSERT(list_get_size(l) == 3);
    ASSERT_INT_EQ(G_AS_INT(list_get_at(l, 0)), 700);
    ASSERT_INT_EQ(G_AS_INT(list_get_at(l, 1)), 500);
    ASSERT_INT_EQ(G_AS_INT(list_get_at(l, 2)), 600);

    list_insert_at(l, 1, G_INT(800));
    ASSERT_INT_EQ(G_AS_INT(list_get_at(l, 0)), 700);
    ASSERT_INT_EQ(G_AS_INT(list_get_at(l, 1)), 800);
    ASSERT_INT_EQ(G_AS_INT(list_get_at(l, 2)), 500);
    ASSERT_INT_EQ(G_AS_INT(list_get_at(l, 3)), 600);

    list_remove_at(l, 1);
    ASSERT_INT_EQ(G_AS_INT(list_get_at(l, 0)), 700);
    ASSERT_INT_EQ(G_AS_INT(list_get_at(l, 1)), 500);
    ASSERT_INT_EQ(G_AS_INT(list_get_at(l, 2)), 600);

    list_remove_at(l, 0);
    ASSERT_INT_EQ(G_AS_INT(list_get_at(l, 0)), 500);
    ASSERT_INT_EQ(G_AS_INT(list_get_at(l, 1)), 600);

    list_remove_at(l, 1);
    ASSERT_INT_EQ(G_AS_INT(list_get_at(l, 0)), 500);
    ASSERT_INT_EQ(list_get_size(l), 1);

    list_remove_at(l, 0);
    ASSERT(list_is_empty(l));

    list_prepend(l, G_INT(8));
    ASSERT_INT_EQ(G_AS_INT(list_get_at(l, 0)), 8);
    ASSERT(!list_is_empty(l));

    list_destroy(l);

    // List copy.
    list_t *l1 = list_create();
    list_t *l2;

    list_append(l1, G_STR("item1"));
    list_append(l1, G_STR("item2"));
    l2 = list_copy(l1);

    char *s1 = list_as_str(l1);
    char *s2 = list_as_str(l2);
    ASSERT_STR_EQ("[\"item1\", \"item2\"]", s1);
    ASSERT_STR_EQ("[\"item1\", \"item2\"]", s2);
    ufree(s1);
    ufree(s2);
    list_destroy(l1);
    list_destroy(l2);

    // List deep copy.
    l1 = list_create();

    list_append(l1, G_STR("item3"));
    list_append(l1, G_STR("item4"));
    l2 = list_copy(l1);

    s1 = list_as_str(l1);
    s2 = list_as_str(l2);
    ASSERT_STR_EQ("[\"item3\", \"item4\"]", s1);
    ASSERT_STR_EQ("[\"item3\", \"item4\"]", s2);
    ufree(s1);
    ufree(s2);
    list_destroy(l1);
    list_destroy(l2);

    // List compare
    l1 = list_create();
    l2 = list_create();
    ASSERT_INT_EQ(list_compare(l1, l2, NULL), 0); // empty list compare equal
    list_append(l1, G_INT(1));
    list_append(l1, G_INT(2));
    list_append(l1, G_INT(3));
    list_append(l2, G_INT(1));
    list_append(l2, G_INT(2));
    list_append(l2, G_INT(3));
    ASSERT_INT_EQ(list_compare(l1, l2, NULL), 0);
    list_append(l1, G_INT(5));
    list_append(l2, G_INT(4));
    ASSERT_INT_EQ(list_compare(l1, l2, NULL), 1);
    ASSERT_INT_EQ(list_compare(l2, l1, NULL), -1);
    list_destroy(l1);
    list_destroy(l2);
}
/*
 * commented out, requires mutating iterator
void test_list_iterator(bool verbose)
{
    (void)verbose;

    char *str;
    vector_t *v = vector_create_empty();
    list_t *l = list_create();

    list_append(l, G_INT(1));
    list_append(l, G_INT(2));
    list_append(l, G_INT(3));
    list_append(l, G_INT(4));

    list_iterator_t *li = list_iterator_create(l);
    while (list_iterator_has_next(li))
    {
        vector_append(v, *list_iterator_get_next(li));
    }
    str = vector_as_str(v);
    if (verbose)
        puts(str);
    ASSERT(strcmp("[1, 2, 3, 4]", str) == 0);
    ufree(str);

    size_t j = 101;
    list_iterator_reset(li);
    while (list_iterator_has_next(li))
    {
        *list_iterator_get_next(li) = G_INT(j++);
    }

    list_iterator_reset(li);
    vector_clear(v);
    while (list_iterator_has_next(li))
    {
        vector_append(v, *list_iterator_get_next(li));
    }
    str = vector_as_str(v);
    if (verbose)
        puts(str);
    ufree(str);

    list_iterator_destroy(li);
    list_destroy(l);
    vector_destroy(v);
}
*/


void test_list_serialize(void)
{
    char *str;
    list_t *l = list_create();

    str = list_as_str(l);
    ASSERT_STR_EQ("[]", str);
    ufree(str);

    list_append(l, G_INT(0));
    str = list_as_str(l);
    ASSERT_STR_EQ("[0]", str);
    ufree(str);

    list_append(l, G_INT(0));
    str = list_as_str(l);
    ASSERT_STR_EQ("[0, 0]", str);
    ufree(str);

    list_append(l, G_INT(1));
    str = list_as_str(l);
    ASSERT_STR_EQ("[0, 0, 1]", str);
    ufree(str);

    list_destroy(l);
}

int main(int argc, char **argv)
{
    (void)argv;
    (void)argc;

    test_list_serialize();
    test_list_api();
//    test_list_iterator(void);

    return 0;
}
