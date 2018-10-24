#include "dict.h"

#include "file_utils.h"
#include "mem.h"
#include "string_utils.h"
#include "ut_utils.h"
#include "vector.h"
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
    out = udict_pop(d, G_STR("k1"), G_NULL());
    UASSERT(!G_IS_NULL(out));
    UASSERT_STR_EQ(G_AS_STR(out), "v1");
    UASSERT_INT_EQ(udict_get_size(d), 2);
    ufree(G_AS_STR(out));

    out = udict_pop(d, G_STR("k2"), G_NULL());
    UASSERT(!G_IS_NULL(out));
    UASSERT_STR_EQ(G_AS_STR(out), "v2");
    UASSERT_INT_EQ(udict_get_size(d), 1);
    ufree(G_AS_STR(out));

    out = udict_pop(d, G_STR("k3"), G_NULL());
    UASSERT(!G_IS_NULL(out));
    UASSERT_STR_EQ(G_AS_STR(out), "v3");
    UASSERT_INT_EQ(udict_get_size(d), 0);
    ufree(G_AS_STR(out));

    UASSERT_G_EQ(udict_pop(d, G_STR("key"), G_STR("-")), G_STR("-"));
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

void test_udict_put(udict_backend_t backend)
{
    udict_t *d = udict_create_with_backend(backend);
    for (size_t i = 0; i < 10; i++)
    {
        udict_put(d, G_INT(i), G_INT(0));
    }
    UASSERT_INT_EQ(udict_get_size(d), 10);

    for (size_t i = 0; i < 10; i++)
    {
        udict_put(d, G_INT(i), G_INT(1));
    }
    UASSERT_INT_EQ(udict_get_size(d), 10);

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
    ugeneric_t out = udict_get(d, gk, G_NULL());
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
    ugeneric_t g = ufile_read_lines(path, "\n");
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

    t = udict_get(d, G_CSTR("---"), G_NULL());
    UASSERT(G_IS_CSTR(t));
    UASSERT_STR_EQ(G_AS_STR(t), "+++");

    t = udict_get(d, G_INT(33), G_NULL());
    UASSERT(G_IS_INT(t));
    UASSERT_INT_EQ(G_AS_INT(t), 2222);

    udict_clear(d);
    udict_put(d, G_CSTR("138586779"), G_INT(1));
    udict_put(d, G_CSTR("138586783"), G_INT(2));

    udict_destroy(d);
}

void test_single(udict_backend_t backend)
{
    ugeneric_t out;

    udict_t *d = udict_create_with_backend(backend);
    UASSERT_INT_EQ(udict_get_size(d), 0);

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
    d = udict_create_with_backend(UDICT_BACKEND_HTBL_WITH_CHAINING);
    udict_put(d, G_INT(1), G_NULL());
    udict_put(d, G_INT(2), G_NULL());
    udict_put(d, G_INT(3), G_NULL());
    udict_put(d, G_INT(4), G_NULL());
    udict_put(d, G_INT(5), G_NULL());
    udict_put(d, G_INT(6), G_NULL());
    udict_put(d, G_INT(-1), G_NULL());
    udict_put(d, G_INT(-2), G_NULL());
    udict_put(d, G_INT(-3), G_NULL());
    udict_put(d, G_INT(-4), G_NULL());
    udict_put(d, G_INT(-5), G_NULL());
    udict_put(d, G_INT(-6), G_NULL());
    udict_put(d, G_INT(0), G_NULL());

    di = udict_iterator_create(d);
    uvector_t *v = uvector_create();
    while (udict_iterator_has_next(di))
    {
        uvector_append(v, (udict_iterator_get_next(di).k));
    }
    uvector_sort(v);
    char *str = ugeneric_as_str(G_VECTOR(v));
    UASSERT_STR_EQ("[-6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6]", str);

    // Reset iterator and iterate again.
    udict_iterator_reset(di);
    while (udict_iterator_has_next(di))
    {
        uvector_append(v, (udict_iterator_get_next(di).k));
    }
    uvector_sort(v);
    ufree(str);
    str = ugeneric_as_str(G_VECTOR(v));
    UASSERT_STR_EQ("[-6, -6, -5, -5, -4, -4, -3, -3, -2, -2, -1, -1, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6]", str);

    udict_iterator_destroy(di);
    udict_destroy(d);
    uvector_destroy(v);
    ufree(str);
}


void _test_udict_cmp(udict_backend_t b1, udict_backend_t b2)
{
    typedef enum {
        UCMP_LESS = -1,
        UCMP_EQUAL = 0,
        UCMP_GREATER = 1,
    } ucmp_result_t;

    typedef struct {
        const char *d1;
        const char *d2;
        ucmp_result_t result;
    } cmp_test_case_t;

    cmp_test_case_t cmp_test_cases[] = {
        {"{}",               "{}",       UCMP_EQUAL},
        {"{'a': 3}",         "{'a': 3}", UCMP_EQUAL},
        {"{'c': 3}",         "{'a': 3}", UCMP_GREATER},
        {"{'a': 3}",         "{'x': 3}", UCMP_LESS},
        {"{'a': 3, 'x': 3}", "{'x': 3}", UCMP_GREATER},
        {"{'a': 3}", "{'a': 3, 'x': 3}", UCMP_LESS},
        {"{'a': 3}",         "{'a': 4}", UCMP_LESS},
        {"{'a': 4}",         "{'a': 3}", UCMP_GREATER},
        {0}
    };

    int r;
    bool fail = false;
    cmp_test_case_t *tc = cmp_test_cases;
    while (tc->d1)
    {
        ugeneric_t g1 = ugeneric_parse(tc->d1);
        ugeneric_t g2 = ugeneric_parse(tc->d2);
        UASSERT(G_IS_DICT(g1));
        UASSERT(G_IS_DICT(g2));

        udict_t *d1 = udict_create_with_backend(b1);
        udict_t *d2 = udict_create_with_backend(b2);
        udict_update(d1, G_AS_PTR(g1));
        udict_update(d2, G_AS_PTR(g2));
        ugeneric_destroy(g1);
        ugeneric_destroy(g2);

        r = udict_compare(d1, d2);
        switch (tc->result)
        {
            case UCMP_LESS:
                fail = (r >= 0);
                break;

            case UCMP_EQUAL:
                fail = (r != 0);
                break;
            case UCMP_GREATER:
                fail = (r <= 0);
                break;
            default:
                UABORT("internal error");
        }

        if (fail)
        {
            udict_print(d1);
            udict_print(d2);
            printf("%d\n", r);
        }

        udict_destroy(d1);
        udict_destroy(d2);

        if (fail)
        {
            UABORT("test failed");
        }

        tc++;
    }

    return;
}

void test_udict_cmp(udict_backend_t backend)
{
    for (int i = 1; i < UDICT_BACKEND_MAX; i++)
    {
        if (i == UDICT_BACKEND_BST_RB)
            continue;
        //printf("%d %d\n", backend, i);
        _test_udict_cmp(backend, i);
    }
}

void test_2sum(void)
{
    const char *path = "utdata/2sum.txt";
 //   const char *path = "utdata/2sum.txt.orig";
    ugeneric_t tmp = ufile_read_lines(path, "\n");
    UASSERT_NO_ERROR(tmp);
    uvector_t *v = G_AS_PTR(tmp);
    size_t vlen = uvector_get_size(v);

    ugeneric_t *cells = uvector_get_cells(v);
    for (size_t i = 0; i < vlen; i++)
    {
        ugeneric_t k = ugeneric_parse(G_AS_STR(cells[i]));
        uvector_set_at(v, i, k);
    }

    for (int backend = 1; backend < UDICT_BACKEND_MAX; backend++)
    {
        //if (backend != UDICT_BACKEND_HTBL_WITH_OPEN_ADDRESSING)
        //    continue;

        udict_t *d = udict_create_with_backend(backend);
        for (size_t i = 0; i < vlen; i++)
        {
            udict_put(d, cells[i], G_NULL());
        }

        int s = 0;
        for (long t = -10000; t < 10000 + 1; t++)
        {
            for (size_t i = 0; i < vlen; i++)
            {
                long k = G_AS_INT(cells[i]);
                if (udict_has_key(d, G_INT(t - k)))
                {
                    if (k != (t - k))
                    {
                        s++;
                        break;
                    }
                }
            }
        }

        udict_destroy(d);
        UASSERT_INT_EQ(s, 4);
        //UASSERT_INT_EQ(s, 427);
        //printf("s: %d\n", s);
    }
    uvector_destroy(v);
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    for (int i = 1; i < UDICT_BACKEND_MAX; i++)
    {
        if (i == UDICT_BACKEND_BST_RB)
            continue;
        test_udict_iterator(i);
        test_udict_const_str(i);
        test_udict_pop(i);
        test_udict_api(i);
        test_udict_serialize(i);
        test_large_dict(i);
        test_single(i);
        test_udict_put(i);
        test_udict_cmp(i);
    }

    test_2sum();
}
