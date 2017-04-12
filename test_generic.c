#include <stdio.h>
#include <string.h>
#include "generic.h"
#include "dict.h"
#include "mem.h"
#include "asserts.h"
#include "file_utils.h"
#include "ut_utils.h"

void test_types(void)
{
    generic_t g;
    g = G_INT(0);
    ASSERT(G_IS_INT(g));

    g = G_MCHUNK(&g, sizeof(g));
    ASSERT(G_IS_MCHUNK(g));
    ASSERT(G_AS_MCHUNK_DATA(g) == &g);
    ASSERT(G_AS_MCHUNK_SIZE(g) == sizeof(g));
    memchunk_t t = {.size = 5, .data = "1234\xff"};
    char *str = memchunk_as_str(t);
    ASSERT_STR_EQ("31323334ff", str);
    ufree(str);
}

void test_generic(void)
{
    //printf("%zu\n", sizeof(generic_t));

    char *str = generic_as_str(G_STR("generic"));
    ASSERT(strcmp(str, "\"generic\"") == 0) ;
    ufree(str);
}

void test_random(void)
{
    while (true)
    {
        printf("%d\n", random_from_range(0, 20));
    }
}

void test_parse(void)
{
    typedef struct {
        const char *in;
        const char *out;
        const char *err;
    } tcase_t;

    tcase_t tc[] = {
        {"[]", "[]", NULL},
        {"{}", "{}", NULL},
        {"[{}]", "[{}]", NULL},
        {"[{},{}]", "[{}, {}]", NULL},
        {"[[],[]]", "[[], []]", NULL},
        {"[[[[[]]]]]","[[[[[]]]]]", NULL},
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
        {"-69.38","-69.38", NULL},
        {"'plata o plomo'","\"plata o plomo\"", NULL},
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
        {0}
    };

    // We need to have the dict to be sorted
    libgnrc_dict_set_default_backend(DICT_BACKEND_BST_RB);

    tcase_t *t = tc;
    while (t->in)
    {
        generic_t g = generic_parse(t->in);
        if (t->out)
        {
            if (G_IS_ERROR(g))
            {
                generic_error_print(g);
                generic_error_destroy(g);
                ABORT("test failed");
            }
            char *out = generic_as_str(g);
            ASSERT_STR_EQ(out, t->out);
            ufree(out);
            //generic_print(g);
            generic_destroy(g, NULL);
        }
        else
        {
            if (!G_IS_ERROR(g))
            {
                //generic_print(g);
            }
            ASSERT(G_IS_ERROR(g));
            //generic_error_print(g);
            if (!string_starts_with(G_AS_STR(g), t->err))
            {
                fprintf(stdout, "'%s' != '%s'\n", G_AS_STR(g), t->err);
                ABORT("test failed");
            }
            generic_error_destroy(g);
        }
        t++;
    }
}

void test_large_parse(void)
{
    const char *path = "utdata/json.json";
    generic_t g = file_read_to_string(path);
    ASSERT_NO_ERROR(g);
    char *json = G_AS_STR(g);
    g = generic_parse(json);
    ASSERT_NO_ERROR(g);
    ufree(json);

    //generic_print(g);

    generic_destroy(g, NULL);
}

void test_serialize(void)
{
    dict_t *d = dict_create(DICT_BACKEND_DEFAULT);
    dict_t *dempty = dict_create(DICT_BACKEND_DEFAULT);
    vector_t *v = vector_create_empty();
    vector_t *vempty = vector_create_empty();
    vector_append(v, G_NULL);
    vector_append(v, G_TRUE);
    vector_append(v, G_FALSE);
    vector_append(v, G_VECTOR(vempty));
    vector_append(v, G_DICT(dempty));
    vector_append(v, G_INT(-1));
    vector_append(v, G_INT(2));
    vector_append(v, G_REAL(3.4));
    vector_append(v, G_SIZE(1888888888888881));
    vector_append(v, G_PTR(NULL));
    vector_set_destroyer(v, ufree);
    dict_put(d, G_CSTR("key"), G_VECTOR(v));

    char *str = generic_as_str(G_DICT(d));
    ASSERT_STR_EQ(str, "{\"key\": [null, true, false, [], {}, -1, 2, 3.4, 1888888888888881, &(nil)]}");
    ufree(str);
    dict_destroy(d);
}

int main(int argc, char **argv)
{
    if (argc > 1)
    {
        generic_t g = file_read_to_string(argv[1]);
        if (G_IS_ERROR(g))
        {
            puts(G_AS_STR(g));
            abort();
        }
        char *str = G_AS_STR(g);
        puts(G_AS_STR(g));
        generic_t res = generic_parse(str);
        if (G_IS_ERROR(res))
        {
            puts(G_AS_STR(res));
        }
        else
        {
            generic_print(res);
        }
    }

    test_types();
    //test_random();
//    test_generic();
    test_parse();
    test_large_parse();
    test_serialize();
}
