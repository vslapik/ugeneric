#include "mem.h"
#include "list.h"
#include "vector.h"
#include "ut_utils.h"
#include "string_utils.h"

void test_ulist_api(void)
{
    ulist_t *l = ulist_create();
    UASSERT(ulist_is_empty(l));

    ulist_append(l, G_INT(1));
    UASSERT(!ulist_is_empty(l));
    ulist_append(l, G_INT(2));
    ulist_append(l, G_INT(3));
    ulist_append(l, G_INT(4));
    UASSERT(ulist_get_size(l) == 4);

    UASSERT_INT_EQ(G_AS_INT(ulist_pop_back(l)), 4);
    UASSERT_INT_EQ(G_AS_INT(ulist_pop_back(l)), 3);
    UASSERT_INT_EQ(G_AS_INT(ulist_pop_back(l)), 2);
    UASSERT_INT_EQ(G_AS_INT(ulist_pop_back(l)), 1);
    UASSERT(ulist_is_empty(l));
    UASSERT(ulist_get_size(l) == 0);

    ulist_clear(l);
    UASSERT(ulist_is_empty(l));
    UASSERT(ulist_get_size(l) == 0);

    ulist_prepend(l, G_INT(11));
    ulist_prepend(l, G_INT(22));
    UASSERT(ulist_get_size(l) == 2);
    UASSERT_INT_EQ(G_AS_INT(ulist_pop_front(l)), 22);
    UASSERT_INT_EQ(G_AS_INT(ulist_pop_front(l)), 11);

    ulist_prepend(l, G_INT(100));
    ulist_append(l, G_INT(200));
    UASSERT_INT_EQ(G_AS_INT(ulist_get_at(l, 0)), 100);
    UASSERT_INT_EQ(G_AS_INT(ulist_get_at(l, 1)), 200);

    ulist_set_at(l, 0, G_INT(500));
    ulist_set_at(l, 1, G_INT(600));
    UASSERT_INT_EQ(G_AS_INT(ulist_get_at(l, 0)), 500);
    UASSERT_INT_EQ(G_AS_INT(ulist_get_at(l, 1)), 600);

    ulist_insert_at(l, 0, G_INT(700));
    UASSERT(ulist_get_size(l) == 3);
    UASSERT_INT_EQ(G_AS_INT(ulist_get_at(l, 0)), 700);
    UASSERT_INT_EQ(G_AS_INT(ulist_get_at(l, 1)), 500);
    UASSERT_INT_EQ(G_AS_INT(ulist_get_at(l, 2)), 600);

    ulist_insert_at(l, 1, G_INT(800));
    UASSERT_INT_EQ(G_AS_INT(ulist_get_at(l, 0)), 700);
    UASSERT_INT_EQ(G_AS_INT(ulist_get_at(l, 1)), 800);
    UASSERT_INT_EQ(G_AS_INT(ulist_get_at(l, 2)), 500);
    UASSERT_INT_EQ(G_AS_INT(ulist_get_at(l, 3)), 600);

    ulist_remove_at(l, 1);
    UASSERT_INT_EQ(G_AS_INT(ulist_get_at(l, 0)), 700);
    UASSERT_INT_EQ(G_AS_INT(ulist_get_at(l, 1)), 500);
    UASSERT_INT_EQ(G_AS_INT(ulist_get_at(l, 2)), 600);

    ulist_remove_at(l, 0);
    UASSERT_INT_EQ(G_AS_INT(ulist_get_at(l, 0)), 500);
    UASSERT_INT_EQ(G_AS_INT(ulist_get_at(l, 1)), 600);

    ulist_remove_at(l, 1);
    UASSERT_INT_EQ(G_AS_INT(ulist_get_at(l, 0)), 500);
    UASSERT_INT_EQ(ulist_get_size(l), 1);

    ulist_remove_at(l, 0);
    UASSERT(ulist_is_empty(l));

    ulist_prepend(l, G_INT(8));
    UASSERT_INT_EQ(G_AS_INT(ulist_get_at(l, 0)), 8);
    UASSERT(!ulist_is_empty(l));

    ulist_destroy(l);

    // List copy.
    ulist_t *l1 = ulist_create();
    ulist_t *l2;

    ulist_append(l1, G_CSTR("item1"));
    ulist_append(l1, G_CSTR("item2"));
    l2 = ulist_copy(l1);

    char *s1 = ulist_as_str(l1);
    char *s2 = ulist_as_str(l2);
    UASSERT_STR_EQ("[\"item1\", \"item2\"]", s1);
    UASSERT_STR_EQ("[\"item1\", \"item2\"]", s2);
    ufree(s1);
    ufree(s2);
    ulist_destroy(l1);
    ulist_destroy(l2);

    // List deep copy.
    l1 = ulist_create();

    ulist_append(l1, G_CSTR("item3"));
    ulist_append(l1, G_CSTR("item4"));
    l2 = ulist_copy(l1);

    s1 = ulist_as_str(l1);
    s2 = ulist_as_str(l2);
    UASSERT_STR_EQ("[\"item3\", \"item4\"]", s1);
    UASSERT_STR_EQ("[\"item3\", \"item4\"]", s2);
    ufree(s1);
    ufree(s2);
    ulist_destroy(l1);
    ulist_destroy(l2);

    // List compare
    l1 = ulist_create();
    l2 = ulist_create();
    UASSERT_INT_EQ(ulist_compare(l1, l2, NULL), 0); // empty list compare equal
    ulist_append(l1, G_INT(1));
    ulist_append(l1, G_INT(2));
    ulist_append(l1, G_INT(3));
    ulist_append(l2, G_INT(1));
    ulist_append(l2, G_INT(2));
    ulist_append(l2, G_INT(3));
    UASSERT_INT_EQ(ulist_compare(l1, l2, NULL), 0);
    ulist_append(l1, G_INT(5));
    ulist_append(l2, G_INT(4));
    UASSERT_INT_EQ(ulist_compare(l1, l2, NULL), 1);
    UASSERT_INT_EQ(ulist_compare(l2, l1, NULL), -1);

    ulist_pop_back(l1);
    UASSERT_INT_EQ(ulist_compare(l1, l2, NULL), -1);
    ulist_pop_back(l2);
    ulist_pop_back(l2);
    UASSERT_INT_EQ(ulist_compare(l1, l2, NULL), 1);

    ulist_destroy(l1);
    ulist_destroy(l2);
}
/*
 * commented out, requires mutating iterator
void test_ulist_iterator(bool verbose)
{
    (void)verbose;

    char *str;
    uvector_t *v = uvector_create_empty();
    ulist_t *l = ulist_create();

    ulist_append(l, G_INT(1));
    ulist_append(l, G_INT(2));
    ulist_append(l, G_INT(3));
    ulist_append(l, G_INT(4));

    ulist_iterator_t *li = ulist_iterator_create(l);
    while (ulist_iterator_has_next(li))
    {
        uvector_append(v, *ulist_iterator_get_next(li));
    }
    str = uvector_as_str(v);
    if (verbose)
        puts(str);
    UASSERT(strcmp("[1, 2, 3, 4]", str) == 0);
    ufree(str);

    size_t j = 101;
    ulist_iterator_reset(li);
    while (ulist_iterator_has_next(li))
    {
        *ulist_iterator_get_next(li) = G_INT(j++);
    }

    ulist_iterator_reset(li);
    uvector_clear(v);
    while (ulist_iterator_has_next(li))
    {
        uvector_append(v, *ulist_iterator_get_next(li));
    }
    str = uvector_as_str(v);
    if (verbose)
        puts(str);
    ufree(str);

    ulist_iterator_destroy(li);
    ulist_destroy(l);
    uvector_destroy(v);
}
*/


void test_ulist_serialize(void)
{
    char *str;
    ulist_t *l = ulist_create();

    str = ulist_as_str(l);
    UASSERT_STR_EQ("[]", str);
    ufree(str);

    ulist_append(l, G_INT(0));
    str = ulist_as_str(l);
    UASSERT_STR_EQ("[0]", str);
    ufree(str);

    ulist_append(l, G_INT(0));
    str = ulist_as_str(l);
    UASSERT_STR_EQ("[0, 0]", str);
    ufree(str);

    ulist_append(l, G_INT(1));
    str = ulist_as_str(l);
    UASSERT_STR_EQ("[0, 0, 1]", str);
    ufree(str);

    ulist_destroy(l);
}

void *_cpy(const void *src)
{
    return ustring_dup(src);
}

void test_list_void_data(void)
{
    ulist_t *l = ulist_create();
    ulist_set_void_destroyer(l, ufree);
    ulist_set_void_copier(l, _cpy);
    ulist_append(l, G_PTR(ustring_dup("1")));
    ulist_append(l, G_PTR(ustring_dup("2")));
    ulist_set_at(l, 0, G_PTR(ustring_dup("11")));

    ulist_t *l2 = ulist_copy(l);
    ulist_t *l3 = ulist_deep_copy(l);

    ulist_destroy(l);
    ulist_destroy(l2);
    ulist_destroy(l3);
}

int main(int argc, char **argv)
{
    (void)argv;
    (void)argc;

    test_ulist_serialize();
    test_ulist_api();
//    test_ulist_iterator(void);
    test_list_void_data();

    return 0;
}
