#include "generic.h"
#include "htbl.h"
#include "mem.h"
#include "string_utils.h"
#include "ut_utils.h"

void test_htbl_api(uhtbl_type_t type)
{
    uhtbl_t *h = uhtbl_create_with_type(type);
    uhtbl_put(h, G_STR(ustring_dup("one")), G_STR(ustring_dup("one")));
    ugeneric_t g = uhtbl_pop(h, G_STR("one"), G_NULL());
    UASSERT(ugeneric_get_type(g) == G_STR_T);
    ufree(G_AS_STR(g));
    uhtbl_destroy(h);

    h = uhtbl_create();
    uhtbl_put(h, G_CSTR("1"), G_CSTR("one"));
    uhtbl_put(h, G_CSTR("2"), G_CSTR("two"));
    uhtbl_put(h, G_STR(ustring_dup("3")), G_STR(ustring_dup("txhree")));

    uvector_t *keys = uhtbl_get_keys(h, false);
    uvector_t *values = uhtbl_get_values(h, false);
    uvector_sort(keys);
    uvector_sort(values);

    char *keys_str = uvector_as_str(keys);
    char *values_str = uvector_as_str(values);
    UASSERT_STR_EQ(keys_str, "[\"1\", \"2\", \"3\"]");
    UASSERT_STR_EQ(values_str, "[\"one\", \"two\", \"txhree\"]");

    ufree(keys_str);
    ufree(values_str);
    uvector_destroy(keys);
    uvector_destroy(values);
    uhtbl_destroy(h);
}

void test_resize(uhtbl_type_t type)
{
    uhtbl_t *h = uhtbl_create_with_type(type);

    for (size_t i = 0; i < 1000; i++)
    {
        uhtbl_put(h, G_INT(i), G_INT(500 + i));
    }

    for (size_t i = 0; i < 1000; i++)
    {
        ugeneric_t g = uhtbl_get(h, G_INT(i), G_NULL());
        UASSERT_INT_EQ(i + 500, G_AS_INT(g));
    }

    uhtbl_destroy(h);
}

int main(void)
{
    test_htbl_api(UHTBL_TYPE_OPEN_ADDRESSING);
    test_htbl_api(UHTBL_TYPE_CHAINING);

    test_resize(UHTBL_TYPE_OPEN_ADDRESSING);
    test_resize(UHTBL_TYPE_CHAINING);
}
