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

void test_dict_pop(dict_backend_t backend)
{
    dict_t *d = dict_create(backend);
    ASSERT(dict_is_empty(d));
    dict_put(d, G_STR(string_dup("k1")), G_STR(string_dup("v1")));
    dict_put(d, G_STR(string_dup("k2")), G_STR(string_dup("v2")));
    dict_put(d, G_STR(string_dup("k3")), G_STR(string_dup("v3")));
    ASSERT(!dict_is_empty(d));
    ASSERT_INT_EQ(dict_get_size(d), 3);
    ASSERT(dict_has_key(d, G_STR("k1")));
    ASSERT(dict_has_key(d, G_STR("k2")));
    ASSERT(dict_has_key(d, G_STR("k3")));
    ASSERT(!dict_has_key(d, G_STR("key")));

    generic_t out;
    out = dict_pop(d, G_STR("k1"), G_NULL);
    ASSERT(!G_IS_NULL(out));
    ASSERT_STR_EQ(G_AS_STR(out), "v1");
    ASSERT_INT_EQ(dict_get_size(d), 2);
    ufree(G_AS_STR(out));

    out = dict_pop(d, G_STR("k2"), G_NULL);
    ASSERT(!G_IS_NULL(out));
    ASSERT_STR_EQ(G_AS_STR(out), "v2");
    ASSERT_INT_EQ(dict_get_size(d), 1);
    ufree(G_AS_STR(out));

    out = dict_pop(d, G_STR("k3"), G_NULL);
    ASSERT(!G_IS_NULL(out));
    ASSERT_STR_EQ(G_AS_STR(out), "v3");
    ASSERT_INT_EQ(dict_get_size(d), 0);
    ufree(G_AS_STR(out));

    ASSERT_STR_EQ(G_AS_STR(dict_pop(d, G_STR("key"), G_STR(""))), "");
    ASSERT(dict_is_empty(d));

    dict_destroy(d);
}

void test_dict_const_str(dict_backend_t backend)
{
    dict_t *d = dict_create(backend);
    dict_put(d, G_CSTR("k1"), G_CSTR("v1"));
    dict_put(d, G_CSTR("k2"), G_CSTR("v2"));
    dict_put(d, G_CSTR("k3"), G_CSTR("v3"));

    ASSERT_STR_EQ(G_AS_STR(dict_get(d, G_CSTR("k1"), G_CSTR(""))), "v1");
    ASSERT_STR_EQ(G_AS_STR(dict_get(d, G_CSTR("k2"), G_CSTR(""))), "v2");
    ASSERT_STR_EQ(G_AS_STR(dict_get(d, G_CSTR("k3"), G_CSTR(""))), "v3");

    dict_clear(d);
    dict_destroy(d);
}

void test_dict_api(dict_backend_t backend)
{
    dict_t *d = dict_create(backend);

    const char *key = "key";
    const char *val = "value";

    generic_t gk, gv;
    gk = G_MCHUNK(string_dup(key), strlen(key) + 1);
    gv = G_MCHUNK(string_dup(val), strlen(val) + 1);
    dict_put(d, gk, gv);
    generic_t out = dict_get(d, gk, G_NULL);
    memchunk_t mout = G_AS_MCHUNK(out);
    ASSERT_STR_EQ(mout.data, "value");

    dict_put(d, G_STR(string_dup("k111")), G_STR(string_dup("v2k111")));
    ASSERT_STR_EQ(G_AS_STR(dict_get(d, G_STR("k111"), G_STR(""))), "v2k111");
    dict_put(d, G_STR(string_dup("k111")), G_STR(string_dup("v3k111")));
    ASSERT_STR_EQ(G_AS_STR(dict_get(d, G_STR("k111"), G_STR(""))), "v3k111");
    dict_put(d, G_STR(string_dup("k2")), G_STR(string_dup("v2aaaaa")));
    ASSERT_STR_EQ(G_AS_STR(dict_get(d, G_STR("k2"), G_STR(""))), "v2aaaaa");
    dict_put(d, G_STR(string_dup("k3")), G_STR(string_dup("v3")));
    dict_put(d, G_STR(string_dup("k4")), G_STR(string_dup("v4")));
    dict_put(d, G_STR(string_dup("k5")), G_STR(string_dup("v5")));
    dict_put(d, G_STR(string_dup("baraban")), G_STR(string_dup("v2aaaaa")));
    dict_put(d, G_STR(string_dup("partizan")), G_STR(string_dup("asdf")));
    dict_put(d, G_STR(string_dup("katamaran")), G_STR(string_dup("asdfnasdfasf")));
    dict_put(d, G_STR(string_dup("veteran")), G_STR(string_dup("asdfasdfasdfasdf")));
    dict_put(d, G_STR(string_dup("pumpamezan")), G_STR(string_dup("vzivoteuragan")));
    dict_clear(d);
    ASSERT_INT_EQ(dict_get_size(d), 0);
    dict_destroy(d);
}

void test_large_dict(dict_backend_t backend)
{
    dict_t *d = dict_create(backend);
    const char *path = "utdata/dict_data.txt";
    generic_t g = file_read_lines(path);
    ASSERT_NO_ERROR(g);
    vector_t *v = G_AS_PTR(g);

    size_t vsize = vector_get_size(v);
    for (size_t i = 0; i < vsize; i++)
    {
       vector_t *v2 = string_split(G_AS_STR(vector_get_at(v, i)), " ");
       vector_drop_data_ownership(v2);
       dict_put(d, vector_get_at(v2, 0), vector_get_at(v2, 1));
       vector_destroy(v2);
    }
    vector_destroy(v);
    //dict_print(d, stdout);
    ASSERT_INT_EQ(500, dict_get_size(d));
    dict_destroy(d);
}

void test_dict_serialize(dict_backend_t backend)
{
    generic_t t;
    char *ds;
    dict_t *d = dict_create(backend);

    ds = dict_as_str(d);
    ASSERT_STR_EQ(ds, "{}");
    ufree(ds);

    dict_put(d, G_CSTR("---"), G_CSTR("+++"));
    ds = dict_as_str(d);
    ASSERT_STR_EQ(ds, "{\"---\": \"+++\"}");
    ufree(ds);
    dict_put(d, G_INT(33), G_INT(2222));

    t = dict_get(d, G_CSTR("---"), G_NULL);
    ASSERT(G_IS_CSTR(t));
    ASSERT_STR_EQ(G_AS_STR(t), "+++");

    t = dict_get(d, G_INT(33), G_NULL);
    ASSERT(G_IS_INT(t));
    ASSERT_INT_EQ(G_AS_INT(t), 2222);

    dict_clear(d);
    dict_put(d, G_CSTR("138586779"), G_INT(1));
    dict_put(d, G_CSTR("138586783"), G_INT(2));

    dict_destroy(d);
}

void test_cfg(dict_backend_t backend)
{
    (void)backend;
    generic_t g = file_read_to_string("cfg");
    ASSERT_NO_ERROR(g);
    char *cfg = G_AS_PTR(g);
    g = generic_parse(cfg);
    //generic_print(g);
    ufree(cfg);
    generic_destroy(g, NULL);
}

void test_singe(dict_backend_t backend)
{
    dict_t *d = dict_create(backend);
    ASSERT_INT_EQ(dict_get_size(d), 0);
    generic_t out;
    dict_put(d, G_STR(string_dup("key")), G_STR(string_dup("value")));
    out = dict_get(d, G_STR("key"), G_STR(""));
    ASSERT_STR_EQ(G_AS_STR(out), "value");
    ASSERT_INT_EQ(dict_get_size(d), 1);
    out = dict_pop(d, G_STR("key"), G_STR(""));
    ASSERT_STR_EQ(G_AS_STR(out), "value");
    ASSERT_INT_EQ(dict_get_size(d), 0);
    ufree(G_AS_STR(out));
    dict_put(d, G_STR(string_dup("key")), G_STR(string_dup("value")));
    ASSERT_INT_EQ(dict_get_size(d), 1);
    dict_clear(d);
    ASSERT_INT_EQ(dict_get_size(d), 0);
    dict_destroy(d);
}

void test_dict_iterator(dict_backend_t backend)
{
    dict_t *d;
    dict_iterator_t *di;
    generic_kv_t out;

    // An empty dict.
    d = dict_create(backend);
    di = dict_iterator_create(d);
    ASSERT(!dict_iterator_has_next(di));
    dict_iterator_reset(di);
    ASSERT(!dict_iterator_has_next(di));
    dict_iterator_destroy(di);
    dict_destroy(d);

    // Single element.
    d = dict_create(backend);
    dict_put(d, G_CSTR("single"), G_CSTR("element"));
    di = dict_iterator_create(d);
    ASSERT(dict_iterator_has_next(di));
    out = dict_iterator_get_next(di);
    ASSERT(!dict_iterator_has_next(di));
    ASSERT_STR_EQ(G_AS_STR(out.k), "single");
    ASSERT_STR_EQ(G_AS_STR(out.v), "element");
    dict_iterator_destroy(di);
    dict_destroy(d);

    // More elements ...
    d = dict_create(DICT_BACKEND_HTBL);
    dict_put(d, G_INT(1), G_NULL);
    dict_put(d, G_INT(2), G_NULL);
    dict_put(d, G_INT(3), G_NULL);
    dict_put(d, G_INT(4), G_NULL);
    dict_put(d, G_INT(5), G_NULL);
    dict_put(d, G_INT(6), G_NULL);
    dict_put(d, G_INT(-1), G_NULL);
    dict_put(d, G_INT(-2), G_NULL);
    dict_put(d, G_INT(-3), G_NULL);
    dict_put(d, G_INT(-4), G_NULL);
    dict_put(d, G_INT(-5), G_NULL);
    dict_put(d, G_INT(-6), G_NULL);
    dict_put(d, G_INT(0), G_NULL);

    di = dict_iterator_create(d);
    vector_t *v = vector_create();
    while (dict_iterator_has_next(di))
    {
        vector_append(v, (dict_iterator_get_next(di).k));
    }
    vector_sort(v);
    char *str = generic_as_str(G_VECTOR(v));
    ASSERT_STR_EQ("[-6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6]", str);

    dict_iterator_destroy(di);
    dict_destroy(d);
    vector_destroy(v);
    ufree(str);
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    return 0;
    for (int i = 0; i < DICT_BACKENDS_COUNT; i++)
    {
        test_dict_iterator(i);
        test_dict_const_str(i);
        test_dict_pop(i);
        test_dict_api(i);
        test_dict_serialize(i);
        test_large_dict(i);
        test_cfg(i);
        test_singe(i);
    }
}
