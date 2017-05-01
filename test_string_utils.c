#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string_utils.h"
#include "ut_utils.h"

static void test_ustring_split(void)
{
    uvector_t *v;

    v = ustring_split("abc bca", " ");
    UASSERT_INT_EQ(uvector_get_size(v), 2);
    UASSERT_STR_EQ(G_AS_STR(uvector_get_at(v, 0)), "abc");
    UASSERT_STR_EQ(G_AS_STR(uvector_get_at(v, 1)), "bca");
    uvector_destroy(v);

    v = ustring_split(" abc bca ", " ");
    UASSERT_INT_EQ(uvector_get_size(v), 4);
    UASSERT_STR_EQ(G_AS_STR(uvector_get_at(v, 0)), "");
    UASSERT_STR_EQ(G_AS_STR(uvector_get_at(v, 1)), "abc");
    UASSERT_STR_EQ(G_AS_STR(uvector_get_at(v, 2)), "bca");
    UASSERT_STR_EQ(G_AS_STR(uvector_get_at(v, 3)), "");
    uvector_destroy(v);

    v = ustring_split("abcabcabc", "z");
    UASSERT_INT_EQ(uvector_get_size(v), 1);
    UASSERT_STR_EQ(G_AS_STR(uvector_get_at(v, 0)), "abcabcabc");
    uvector_destroy(v);

    v = ustring_split("abcabcabc", "abc");
    UASSERT_INT_EQ(uvector_get_size(v), 4);
    UASSERT_STR_EQ(G_AS_STR(uvector_get_at(v, 0)), "");
    UASSERT_STR_EQ(G_AS_STR(uvector_get_at(v, 1)), "");
    UASSERT_STR_EQ(G_AS_STR(uvector_get_at(v, 2)), "");
    UASSERT_STR_EQ(G_AS_STR(uvector_get_at(v, 3)), "");
    uvector_destroy(v);

    v = ustring_split("abcabcabc", "bc");
    UASSERT_INT_EQ(uvector_get_size(v), 4);
    UASSERT_STR_EQ(G_AS_STR(uvector_get_at(v, 0)), "a");
    UASSERT_STR_EQ(G_AS_STR(uvector_get_at(v, 1)), "a");
    UASSERT_STR_EQ(G_AS_STR(uvector_get_at(v, 2)), "a");
    UASSERT_STR_EQ(G_AS_STR(uvector_get_at(v, 3)), "");
    uvector_destroy(v);
}

void test_ustring_fmt(void)
{
    char *str;
    size_t size;

    str = ustring_fmt("%s%s%s%c", "Hello", " ", "world", '!');
    UASSERT_STR_EQ(str, "Hello world!");
    ufree(str);

    str = ustring_fmt_sized("%s%s%s%c", &size, "Hello", " ", "world", '!');
    UASSERT_STR_EQ(str, "Hello world!");
    UASSERT_SIZE_EQ(size, 12);
    ufree(str);

    /* TODO: clarify behaviour
    str = ustring_fmt("%s", "");
    UASSERT_STR_EQ(str, "");
    ufree(str);
    */
}

int main(void)
{
    test_ustring_split();
    test_ustring_fmt();

    return EXIT_SUCCESS;
}
