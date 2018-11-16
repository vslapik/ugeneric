#include "generic.h"

#include "dict.h"
#include "file_utils.h"
#include "mem.h"
#include "ut_utils.h"
#include "vector.h"
#include <limits.h>

void test_types(void)
{
    char *m = ustring_dup("memchunk");
    ugeneric_t g_integer = G_INT(333);
    ugeneric_t g_real = G_REAL(33.33);
    ugeneric_t g_string = G_STR(ustring_dup("hello ugeneric"));
    ugeneric_t g_cstring = G_CSTR("hello constant ugeneric");
    ugeneric_t g_size = G_SIZE(123412341);
    ugeneric_t g_null = G_NULL();
    ugeneric_t g_true = G_TRUE();
    ugeneric_t g_false = G_FALSE();
    ugeneric_t g_vector = G_VECTOR(uvector_create());
    ugeneric_t g_dict = G_DICT(udict_create());
    ugeneric_t g_memchunk = G_MEMCHUNK(m, strlen(m));

    UASSERT(G_IS_INT(g_integer));
    UASSERT(G_IS_REAL(g_real));
    UASSERT(G_IS_STR(g_string));
    UASSERT(G_IS_CSTR(g_cstring));
    UASSERT(G_IS_STRING(g_cstring));
    UASSERT(G_IS_STRING(g_string));
    UASSERT(G_IS_SIZE(g_size));
    UASSERT(G_IS_NULL(g_null));
    UASSERT(G_IS_TRUE(g_true));
    UASSERT(G_IS_FALSE(g_false));
    UASSERT(G_IS_VECTOR(g_vector));
    UASSERT(G_IS_DICT(g_dict));
    UASSERT(G_IS_MEMCHUNK(g_memchunk));
    UASSERT(G_AS_MEMCHUNK_DATA(g_memchunk) == m);
    UASSERT(G_AS_MEMCHUNK_SIZE(g_memchunk) == strlen(m));

    ugeneric_t g_integer_copy = ugeneric_copy(g_integer);
    ugeneric_t g_real_copy = ugeneric_copy(g_real);
    ugeneric_t g_string_copy = ugeneric_copy(g_string);
    ugeneric_t g_cstring_copy = ugeneric_copy(g_cstring);
    ugeneric_t g_size_copy = ugeneric_copy(g_size);
    ugeneric_t g_null_copy = ugeneric_copy(g_null);
    ugeneric_t g_true_copy = ugeneric_copy(g_true);
    ugeneric_t g_false_copy = ugeneric_copy(g_false);
    ugeneric_t g_vector_copy = ugeneric_copy(g_vector);
    ugeneric_t g_dict_copy = ugeneric_copy(g_dict);
    ugeneric_t g_memchunk_copy = ugeneric_copy(g_memchunk);

    UASSERT(G_IS_INT(g_integer_copy));
    UASSERT(G_IS_REAL(g_real_copy));
    UASSERT(G_IS_STR(g_string_copy));
    UASSERT(G_IS_STR(g_cstring_copy)); // copying G_CSTR creates G_STR
    UASSERT(G_IS_STRING(g_cstring_copy));
    UASSERT(G_IS_STRING(g_string_copy));
    UASSERT(G_IS_SIZE(g_size_copy));
    UASSERT(G_IS_NULL(g_null_copy));
    UASSERT(G_IS_TRUE(g_true_copy));
    UASSERT(G_IS_FALSE(g_false_copy));
    UASSERT(G_IS_VECTOR(g_vector_copy));
    UASSERT(G_IS_DICT(g_dict_copy));
    UASSERT(G_IS_MEMCHUNK(g_memchunk_copy));
    UASSERT(G_AS_MEMCHUNK_DATA(g_memchunk_copy) != m);
    UASSERT(G_AS_MEMCHUNK_SIZE(g_memchunk_copy) == strlen(m));

    UASSERT(ugeneric_compare(g_integer, g_integer_copy) == 0);
    UASSERT(ugeneric_compare(g_real, g_real_copy) == 0);
    UASSERT(ugeneric_compare(g_string, g_string_copy) == 0);
    UASSERT(ugeneric_compare(g_cstring, g_cstring_copy) == 0);
    UASSERT(ugeneric_compare(g_size, g_size_copy) == 0);
    UASSERT(ugeneric_compare(g_null, g_null_copy) == 0);
    UASSERT(ugeneric_compare(g_true, g_true_copy) == 0);
    UASSERT(ugeneric_compare(g_false, g_false_copy) == 0);
    UASSERT(ugeneric_compare(g_vector, g_vector_copy) == 0);
    UASSERT(ugeneric_compare(g_dict, g_dict_copy) == 0);
    UASSERT(ugeneric_compare(g_memchunk, g_memchunk_copy) == 0);

    UASSERT(ugeneric_compare(g_memchunk, g_integer) != 0);

    ugeneric_destroy(g_integer);
    ugeneric_destroy(g_real);
    ugeneric_destroy(g_string);
    ugeneric_destroy(g_cstring);
    ugeneric_destroy(g_size);
    ugeneric_destroy(g_null);
    ugeneric_destroy(g_true);
    ugeneric_destroy(g_false);
    ugeneric_destroy(g_vector);
    ugeneric_destroy(g_dict);
    ugeneric_destroy(g_memchunk);

    ugeneric_destroy(g_integer_copy);
    ugeneric_destroy(g_real_copy);
    ugeneric_destroy(g_string_copy);
    ugeneric_destroy(g_cstring_copy);
    ugeneric_destroy(g_size_copy);
    ugeneric_destroy(g_null_copy);
    ugeneric_destroy(g_true_copy);
    ugeneric_destroy(g_false_copy);
    ugeneric_destroy(g_vector_copy);
    ugeneric_destroy(g_dict_copy);
    ugeneric_destroy(g_memchunk_copy);
}

void test_generic_cmp(void)
{
    UASSERT(ugeneric_compare(G_INT(-1), G_INT(1)) < 0);
    UASSERT(ugeneric_compare(G_INT(1), G_INT(-1)) > 0);
    UASSERT(ugeneric_compare(G_INT(0), G_INT(0)) == 0);
    UASSERT(ugeneric_compare(G_INT(LONG_MAX), G_INT(LONG_MIN)) > 0);
    UASSERT(ugeneric_compare(G_INT(LONG_MIN), G_INT(LONG_MAX)) < 0);
    UASSERT(ugeneric_compare(G_INT(LONG_MAX), G_INT(LONG_MAX)) == 0);
    UASSERT(ugeneric_compare(G_INT(LONG_MIN), G_INT(LONG_MIN)) == 0);

    UASSERT(ugeneric_compare(G_INT(1), G_INT(2)) < 0);
    UASSERT(ugeneric_compare(G_INT(2), G_INT(1)) > 0);
    UASSERT(ugeneric_compare(G_INT(0), G_INT(0)) == 0);
    UASSERT(ugeneric_compare(G_SIZE(ULONG_MAX), G_SIZE(0)) > 0);
    UASSERT(ugeneric_compare(G_SIZE(0), G_SIZE(ULONG_MAX)) < 0);
    UASSERT(ugeneric_compare(G_INT(ULONG_MAX), G_INT(ULONG_MAX)) == 0);
}

void test_generic(void)
{
    //printf("%zu\n", sizeof(ugeneric_t));

    char *str = ugeneric_as_str(G_STR("generic"));
    UASSERT(strcmp(str, "\"generic\"") == 0) ;
    ufree(str);
}

void test_random(void)
{
    #define N 10000

    uvector_t *v = uvector_create_with_size(21, G_INT(0));
    for (size_t i = 0; i < N; i++)
    {
        int n = ugeneric_random_from_range(-10, 10);
        UASSERT(n >= -10);
        UASSERT(n <= 10);
        uvector_set_at(v, n + 10, G_INT(G_AS_INT(uvector_get_at(v, n + 10)) + 1));
    }

    //uvector_print(v);
    uvector_sort(v);

    int diff = G_AS_INT(uvector_get_front(v)) - G_AS_INT(uvector_get_back(v));
    //printf("diff is %d\n", diff);

    UASSERT(diff < N/33); // let's at least ask for 5%

    uvector_destroy(v);
}

void test_parse(void)
{
    typedef struct {
        const char *in;
        const char *out;
        const char *err;
    } tcase_t;

    tcase_t tc[] = {
//        {"\"str\0ing\"", "\"string\"", NULL},
        {"[]", "[]", NULL},
        {"{}", "{}", NULL},
        {"[{}]", "[{}]", NULL},
        {"[{},{}]", "[{}, {}]", NULL},
        {"[[],[]]", "[[], []]", NULL},
        {"[[[[[]]]]]", "[[[[[]]]]]", NULL},
        {"[1]", "[1]", NULL},
        {"[1,2,3]", "[1, 2, 3]", NULL},
        {"[1,2,3,]", "[1, 2, 3]", NULL},
        {"{ }", "{}", NULL},
        {"[ ]", "[]", NULL},
        {"\"t\\\"tt\"", "\"t\\\"tt\"", NULL},
        {"\"str'ing\"", "\"str'ing\"", NULL},
        {"\"\\\"\\\"\\\"\"", "\"\\\"\\\"\\\"\"", NULL},
        {"\"\\\\\\\\\"", "\"\\\\\"", NULL},
        {"[ ]", "[]", NULL},
        {"{ }", "{}", NULL},
        {"null", "null", NULL},
        {"true", "true", NULL},
        {"false", "false", NULL},
        {"\"str\"", "\"str\"", NULL},
        {"12345", "12345", NULL},
        {"-69.38", "-69.38", NULL},
        {"'plata o plomo'", "\"plata o plomo\"", NULL},
        {"[1,2,3,4]", "[1, 2, 3, 4]", NULL},
        {"{1:2}", "{1: 2}", NULL},
        {"{1:2, true: false}", "{1: 2, true: false}", NULL},
        {"{1: {2: {true: false}}}", "{1: {2: {true: false}}}", NULL},
        {"{1:[1], 2:[2]}", "{1: [1], 2: [2]}", NULL},
        {"1.2E34", "1.2e+34", NULL},
        {"[", NULL, "Parsing failed at offset 1"},
        {"[],", NULL, "Parsing failed at offset 2"},
        {"{},", NULL, "Parsing failed at offset 2"},
        {",", NULL, "Parsing failed at offset 0"},
        {"[0,,]", NULL, "Parsing failed at offset 3"},
        {"null,", NULL, "Parsing failed at offset 4"},
        {"\"str", NULL, "Parsing failed at offset 4"},
        {"[{]}", NULL, "Parsing failed at offset 2"},
        {"[1,2,}", NULL, "Parsing failed at offset 5"},
        {"{1,2,}", NULL, "Parsing failed at offset 2"},
        {"]", NULL, "Parsing failed at offset 0"},
        {"}", NULL, "Parsing failed at offset 0"},
        {"a", NULL, "Parsing failed at offset 0"},
        {"&", NULL, "Parsing failed at offset 0"},
        {"", NULL, "Parsing failed at offset 0"},
        {"{true: {false: [];}}", NULL, "Parsing failed at offset 17"},
        {"1 ", "1", NULL},
        {"1.01 ", "1.01", NULL},
        {"1\n", "1", NULL},
        {"1.01\n", "1.01", NULL},
        {"1\t", "1", NULL},
        {"-", NULL, "Parsing failed at offset 0"},
        {"[-]", NULL, "Parsing failed at offset 1"},
        {"[-3-]", NULL, "Parsing failed at offset 3"},
        {"--3", NULL, "Parsing failed at offset 0"},
        {"0.0", "0", NULL},
        {"-0.0", "-0", NULL},
        {"1.0", "1", NULL},
        {"-1.0", "-1", NULL},
        {"1.5", "1.5", NULL},
        {"-1.5", "-1.5", NULL},
        {"3.1416", "3.1416", NULL},
        {"2E20", "2e+20", NULL},
        {"2e20", "2e+20", NULL},
        {"2E+20", "2e+20", NULL},
        {"2E-20", "2e-20", NULL},
        {"-1E10", "-1e+10", NULL},
        {"-1e10", "-1e+10", NULL},
        {"-1E+10", "-1e+10", NULL},
        {"-1E-10", "-1e-10", NULL},
        {"1.234E+10", "1.234e+10", NULL},
        {"1.234E-10", "1.234e-10", NULL},
        {"0.9868011474609375", "0.986801", NULL},
        {"45913141877270640000.0", "4.59131e+19", NULL},
        {"0.017976931348623157e+310", "1.79769e+308", NULL},
        {"5708990770823839207320493820740630171355185152001e-3", "5.70899e+45", NULL},
        {"mem:000011ccFFaa", "mem:000011ccffaa", NULL},
        {"mem:", "mem:", NULL},
        {0}
    };

    // We need to have the dict to be sorted
    libugeneric_udict_set_default_backend(UDICT_BACKEND_BST_RB);

    tcase_t *t = tc;
    while (t->in)
    {
        ugeneric_t g = ugeneric_parse(t->in);
        if (t->out)
        {
            if (G_IS_ERROR(g))
            {
                ugeneric_error_print(g);
                ugeneric_error_destroy(g);
                UABORT("test failed");
            }
            char *out = ugeneric_as_str(g);
            UASSERT_STR_EQ(out, t->out);
            ufree(out);
            //ugeneric_print(g);
            ugeneric_destroy(g);
        }
        else
        {
            if (!G_IS_ERROR(g))
            {
                //ugeneric_print(g);
            }
            UASSERT(G_IS_ERROR(g));
            //ugeneric_error_print(g);
            if (!ustring_starts_with(G_AS_STR(g), t->err))
            {
                fprintf(stdout, "'%s' != '%s'\n", G_AS_STR(g), t->err);
                UABORT("test failed");
            }
            ugeneric_error_destroy(g);
        }
        t++;
    }
}

void test_large_parse(void)
{
    const char *path = "utdata/json.json";
    ugeneric_t g = ufile_read_to_string(path);
    UASSERT_NO_ERROR(g);
    char *json = G_AS_STR(g);
    g = ugeneric_parse(json);
    UASSERT_NO_ERROR(g);
    ufree(json);

    //ugeneric_print(g);

    ugeneric_destroy(g);
}

void test_serialize(void)
{
    udict_t *d = udict_create();
    udict_t *dempty = udict_create();
    uvector_t *v = uvector_create();
    uvector_t *vempty = uvector_create();
    uvector_append(v, G_NULL());
    uvector_append(v, G_TRUE());
    uvector_append(v, G_FALSE());
    uvector_append(v, G_VECTOR(vempty));
    uvector_append(v, G_DICT(dempty));
    uvector_append(v, G_INT(-1));
    uvector_append(v, G_INT(2));
    uvector_append(v, G_REAL(3.4));
    uvector_append(v, G_SIZE(188881));
    uvector_append(v, G_MEMCHUNK(umemdup("\xff\xaa\xbb", 3), 3));
    uvector_set_void_destroyer(v, ufree);
    udict_put(d, G_CSTR("key"), G_VECTOR(v));

    char *str = ugeneric_as_str(G_DICT(d));
    UASSERT_STR_EQ(str, "{\"key\": [null, true, false, [], {}, -1, 2, 3.4, 188881, mem:ffaabb]}");
    ufree(str);
    udict_destroy(d);
}

void test_parse_size(void)
{
    char *integer = ustring_fmt("%ld", LONG_MAX);
    char *size = ustring_fmt("%zu", ULONG_MAX); // don't fit to long int
    char *over_size1 = ustring_fmt("-%s", size); // don't fit anywhere
    char *over_size2 = ustring_fmt("1%s", size); // don't fit anywhere

    ugeneric_t i = ugeneric_parse(integer);
    UASSERT_NO_ERROR(i);
    UASSERT(G_IS_INT(i));
    UASSERT_INT_EQ(G_AS_INT(i), LONG_MAX);

    ugeneric_t s = ugeneric_parse(size);
    UASSERT_NO_ERROR(s);
    UASSERT(G_IS_SIZE(s));
    UASSERT_SIZE_EQ(G_AS_SIZE(s), ULONG_MAX);

    ugeneric_t os1 = ugeneric_parse(over_size1);
    UASSERT(G_IS_ERROR(os1));
    ugeneric_error_destroy(os1);

    ugeneric_t os2 = ugeneric_parse(over_size2);
    UASSERT(G_IS_ERROR(os2));
    ugeneric_error_destroy(os2);

    ufree(integer);
    ufree(size);
    ufree(over_size1);
    ufree(over_size2);
}

int main(int argc, char **argv)
{

    if (argc > 1)
    {
        ugeneric_t g = ufile_read_to_string(argv[1]);
        if (G_IS_ERROR(g))
        {
            puts(G_AS_STR(g));
            abort();
        }
        char *str = G_AS_STR(g);
        puts(G_AS_STR(g));
        ugeneric_t res = ugeneric_parse(str);
        if (G_IS_ERROR(res))
        {
            puts(G_AS_STR(res));
        }
        else
        {
            ugeneric_print(res);
        }
    }

    test_types();
    test_random();
    test_generic();
    test_parse();
    test_large_parse();
    test_serialize();
    test_parse_size();
    test_generic_cmp();
}
