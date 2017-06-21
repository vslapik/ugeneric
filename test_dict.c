#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "mem.h"
#include "dict.h"
#include "vector.h"
#include "string_utils.h"
#include "file_utils.h"
#include "ut_utils.h"
#include "vector.h"

void test_udict_pop(udict_backend_t backend)
{
    udict_t *d = udict_create_with_backend(backend);
    UASSERT(udict_is_empty(d));
    udict_put(d, G_STR(ustring_dup("k1")), G_STR(ustring_dup("v1")));
    udict_put(d, G_STR(ustring_dup("k2")), G_STR(ustring_dup("v2")));
    udict_put(d, G_STR(ustring_dup("k3")), G_STR(ustring_dup("v3")));
    UASSERT(!udict_is_empty(d));
    UASSERT_INT_EQ(udict_get_size(d), 3);
    UASSERT(udict_has_key(d, G_STR("k1")));
    UASSERT(udict_has_key(d, G_STR("k2")));
    UASSERT(udict_has_key(d, G_STR("k3")));
    UASSERT(!udict_has_key(d, G_STR("key")));

    ugeneric_t out;
    out = udict_pop(d, G_STR("k1"), G_NULL);
    UASSERT(!G_IS_NULL(out));
    UASSERT_STR_EQ(G_AS_STR(out), "v1");
    UASSERT_INT_EQ(udict_get_size(d), 2);
    ufree(G_AS_STR(out));

    out = udict_pop(d, G_STR("k2"), G_NULL);
    UASSERT(!G_IS_NULL(out));
    UASSERT_STR_EQ(G_AS_STR(out), "v2");
    UASSERT_INT_EQ(udict_get_size(d), 1);
    ufree(G_AS_STR(out));

    out = udict_pop(d, G_STR("k3"), G_NULL);
    UASSERT(!G_IS_NULL(out));
    UASSERT_STR_EQ(G_AS_STR(out), "v3");
    UASSERT_INT_EQ(udict_get_size(d), 0);
    ufree(G_AS_STR(out));

    UASSERT_STR_EQ(G_AS_STR(udict_pop(d, G_STR("key"), G_STR(""))), "");
    UASSERT(udict_is_empty(d));

    udict_destroy(d);
}

void test_udict_const_str(udict_backend_t backend)
{
    udict_t *d = udict_create_with_backend(backend);
    udict_put(d, G_CSTR("k1"), G_CSTR("v1"));
    udict_put(d, G_CSTR("k2"), G_CSTR("v2"));
    udict_put(d, G_CSTR("k3"), G_CSTR("v3"));

    UASSERT_STR_EQ(G_AS_STR(udict_get(d, G_CSTR("k1"), G_CSTR(""))), "v1");
    UASSERT_STR_EQ(G_AS_STR(udict_get(d, G_CSTR("k2"), G_CSTR(""))), "v2");
    UASSERT_STR_EQ(G_AS_STR(udict_get(d, G_CSTR("k3"), G_CSTR(""))), "v3");

    udict_clear(d);
    udict_destroy(d);
}

void test_udict_api(udict_backend_t backend)
{
    udict_t *d = udict_create_with_backend(backend);

    const char *key = "key";
    const char *val = "value";

    ugeneric_t gk, gv;
    gk = G_MEMCHUNK(ustring_dup(key), strlen(key) + 1);
    gv = G_MEMCHUNK(ustring_dup(val), strlen(val) + 1);
    udict_put(d, gk, gv);
    ugeneric_t out = udict_get(d, gk, G_NULL);
    umemchunk_t mout = G_AS_MEMCHUNK(out);
    UASSERT_STR_EQ(mout.data, "value");

    udict_put(d, G_STR(ustring_dup("k111")), G_STR(ustring_dup("v2k111")));
    UASSERT_STR_EQ(G_AS_STR(udict_get(d, G_STR("k111"), G_STR(""))), "v2k111");
    udict_put(d, G_STR(ustring_dup("k111")), G_STR(ustring_dup("v3k111")));
    UASSERT_STR_EQ(G_AS_STR(udict_get(d, G_STR("k111"), G_STR(""))), "v3k111");
    udict_put(d, G_STR(ustring_dup("k2")), G_STR(ustring_dup("v2aaaaa")));
    UASSERT_STR_EQ(G_AS_STR(udict_get(d, G_STR("k2"), G_STR(""))), "v2aaaaa");
    udict_put(d, G_STR(ustring_dup("k3")), G_STR(ustring_dup("v3")));
    udict_put(d, G_STR(ustring_dup("k4")), G_STR(ustring_dup("v4")));
    udict_put(d, G_STR(ustring_dup("k5")), G_STR(ustring_dup("v5")));
    udict_put(d, G_STR(ustring_dup("baraban")), G_STR(ustring_dup("v2aaaaa")));
    udict_put(d, G_STR(ustring_dup("partizan")), G_STR(ustring_dup("asdf")));
    udict_put(d, G_STR(ustring_dup("katamaran")), G_STR(ustring_dup("asdfnasdfasf")));
    udict_put(d, G_STR(ustring_dup("veteran")), G_STR(ustring_dup("asdfasdfasdfasdf")));
    udict_put(d, G_STR(ustring_dup("pumpamezan")), G_STR(ustring_dup("vzivoteuragan")));
    udict_clear(d);
    UASSERT_INT_EQ(udict_get_size(d), 0);
    udict_destroy(d);
}

void test_large_dict(udict_backend_t backend)
{
    udict_t *d = udict_create_with_backend(backend);
    const char *path = "utdata/dict_data.txt";
    ugeneric_t g = ufile_read_lines(path);
    UASSERT_NO_ERROR(g);
    uvector_t *v = G_AS_PTR(g);

    size_t vsize = uvector_get_size(v);
    for (size_t i = 0; i < vsize; i++)
    {
       uvector_t *v2 = ustring_split(G_AS_STR(uvector_get_at(v, i)), " ");
       uvector_drop_data_ownership(v2);
       udict_put(d, uvector_get_at(v2, 0), uvector_get_at(v2, 1));
       uvector_destroy(v2);
    }
    uvector_destroy(v);
    //udict_print(d, stdout);
    UASSERT_INT_EQ(500, udict_get_size(d));
    udict_destroy(d);
}

void test_udict_serialize(udict_backend_t backend)
{
    ugeneric_t t;
    char *ds;
    udict_t *d = udict_create_with_backend(backend);

    ds = udict_as_str(d);
    UASSERT_STR_EQ(ds, "{}");
    ufree(ds);

    udict_put(d, G_CSTR("---"), G_CSTR("+++"));
    ds = udict_as_str(d);
    UASSERT_STR_EQ(ds, "{\"---\": \"+++\"}");
    ufree(ds);
    udict_put(d, G_INT(33), G_INT(2222));

    t = udict_get(d, G_CSTR("---"), G_NULL);
    UASSERT(G_IS_CSTR(t));
    UASSERT_STR_EQ(G_AS_STR(t), "+++");

    t = udict_get(d, G_INT(33), G_NULL);
    UASSERT(G_IS_INT(t));
    UASSERT_INT_EQ(G_AS_INT(t), 2222);

    udict_clear(d);
    udict_put(d, G_CSTR("138586779"), G_INT(1));
    udict_put(d, G_CSTR("138586783"), G_INT(2));

    udict_destroy(d);
}

void test_singe(udict_backend_t backend)
{
    udict_t *d = udict_create_with_backend(backend);
    UASSERT_INT_EQ(udict_get_size(d), 0);
    ugeneric_t out;
    udict_put(d, G_STR(ustring_dup("key")), G_STR(ustring_dup("value")));
    out = udict_get(d, G_STR("key"), G_STR(""));
    UASSERT_STR_EQ(G_AS_STR(out), "value");
    UASSERT_INT_EQ(udict_get_size(d), 1);
    out = udict_pop(d, G_STR("key"), G_STR(""));
    UASSERT_STR_EQ(G_AS_STR(out), "value");
    UASSERT_INT_EQ(udict_get_size(d), 0);
    ufree(G_AS_STR(out));
    udict_put(d, G_STR(ustring_dup("key")), G_STR(ustring_dup("value")));
    UASSERT_INT_EQ(udict_get_size(d), 1);
    udict_clear(d);
    UASSERT_INT_EQ(udict_get_size(d), 0);
    udict_destroy(d);
}

void test_udict_iterator(udict_backend_t backend)
{
    udict_t *d;
    udict_iterator_t *di;
    ugeneric_kv_t out;

    // An empty dict.
    d = udict_create_with_backend(backend);
    di = udict_iterator_create(d);
    UASSERT(!udict_iterator_has_next(di));
    udict_iterator_reset(di);
    UASSERT(!udict_iterator_has_next(di));
    udict_iterator_destroy(di);
    udict_destroy(d);

    // Single element.
    d = udict_create_with_backend(backend);
    udict_put(d, G_CSTR("single"), G_CSTR("element"));
    di = udict_iterator_create(d);
    UASSERT(udict_iterator_has_next(di));
    out = udict_iterator_get_next(di);
    UASSERT(!udict_iterator_has_next(di));
    UASSERT_STR_EQ(G_AS_STR(out.k), "single");
    UASSERT_STR_EQ(G_AS_STR(out.v), "element");
    udict_iterator_destroy(di);
    udict_destroy(d);

    // More elements ...
    d = udict_create_with_backend(UDICT_BACKEND_HTBL);
    udict_put(d, G_INT(1), G_NULL);
    udict_put(d, G_INT(2), G_NULL);
    udict_put(d, G_INT(3), G_NULL);
    udict_put(d, G_INT(4), G_NULL);
    udict_put(d, G_INT(5), G_NULL);
    udict_put(d, G_INT(6), G_NULL);
    udict_put(d, G_INT(-1), G_NULL);
    udict_put(d, G_INT(-2), G_NULL);
    udict_put(d, G_INT(-3), G_NULL);
    udict_put(d, G_INT(-4), G_NULL);
    udict_put(d, G_INT(-5), G_NULL);
    udict_put(d, G_INT(-6), G_NULL);
    udict_put(d, G_INT(0), G_NULL);

    di = udict_iterator_create(d);
    uvector_t *v = uvector_create();
    while (udict_iterator_has_next(di))
    {
        uvector_append(v, (udict_iterator_get_next(di).k));
    }
    uvector_sort(v);
    char *str = ugeneric_as_str(G_VECTOR(v), NULL);
    UASSERT_STR_EQ("[-6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6]", str);

    udict_iterator_destroy(di);
    udict_destroy(d);
    uvector_destroy(v);
    ufree(str);
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    int i;
    //for (int i = 0; i < UDICT_BACKENDS_COUNT; i++)
    i = UDICT_BACKEND_HTBL;
    {
        test_udict_iterator(i);
        test_udict_const_str(i);
        test_udict_pop(i);
        test_udict_api(i);
        test_udict_serialize(i);
        test_large_dict(i);
        test_singe(i);
    }

    i = UDICT_BACKEND_UBST_PLAIN;
    {
        test_udict_iterator(i);
        test_udict_const_str(i);
        test_udict_pop(i);
        test_udict_api(i);
        test_udict_serialize(i);
        test_large_dict(i);
        test_singe(i);
    }
}
