#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string_utils.h"
#include "ut_utils.h"

static void test_string_split(void)
{
    vector_t *v;

    v = string_split("abc bca", " ");
    ASSERT_INT_EQ(vector_get_size(v), 2);
    ASSERT_STR_EQ(G_AS_STR(vector_get_at(v, 0)), "abc");
    ASSERT_STR_EQ(G_AS_STR(vector_get_at(v, 1)), "bca");
    vector_destroy(v);

    v = string_split(" abc bca ", " ");
    ASSERT_INT_EQ(vector_get_size(v), 4);
    ASSERT_STR_EQ(G_AS_STR(vector_get_at(v, 0)), "");
    ASSERT_STR_EQ(G_AS_STR(vector_get_at(v, 1)), "abc");
    ASSERT_STR_EQ(G_AS_STR(vector_get_at(v, 2)), "bca");
    ASSERT_STR_EQ(G_AS_STR(vector_get_at(v, 3)), "");
    vector_destroy(v);

    v = string_split("abcabcabc", "z");
    ASSERT_INT_EQ(vector_get_size(v), 1);
    ASSERT_STR_EQ(G_AS_STR(vector_get_at(v, 0)), "abcabcabc");
    vector_destroy(v);

    v = string_split("abcabcabc", "abc");
    ASSERT_INT_EQ(vector_get_size(v), 4);
    ASSERT_STR_EQ(G_AS_STR(vector_get_at(v, 0)), "");
    ASSERT_STR_EQ(G_AS_STR(vector_get_at(v, 1)), "");
    ASSERT_STR_EQ(G_AS_STR(vector_get_at(v, 2)), "");
    ASSERT_STR_EQ(G_AS_STR(vector_get_at(v, 3)), "");
    vector_destroy(v);

    v = string_split("abcabcabc", "bc");
    ASSERT_INT_EQ(vector_get_size(v), 4);
    ASSERT_STR_EQ(G_AS_STR(vector_get_at(v, 0)), "a");
    ASSERT_STR_EQ(G_AS_STR(vector_get_at(v, 1)), "a");
    ASSERT_STR_EQ(G_AS_STR(vector_get_at(v, 2)), "a");
    ASSERT_STR_EQ(G_AS_STR(vector_get_at(v, 3)), "");
    vector_destroy(v);

}

int main(void)
{
    test_string_split();
    return EXIT_SUCCESS;
}
