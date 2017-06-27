#include <limits.h>
#include "ut_utils.h"

ugeneric_t gen_random_generic(int depth)
{
    ugeneric_t g = G_CSTR("g");
    if (depth > 0)
    {
        ugeneric_type_e gtype;
        gtype = ugeneric_random_from_range(G_NULL_T, G_MEMCHUNK_T);
        UASSERT(gtype > G_ERROR_T);
        UASSERT(gtype <= G_MEMCHUNK_T);
        switch(gtype)
        {
            case G_NULL_T:     g = G_NULL; break;
            case G_PTR_T:      break;
            case G_STR_T:      g = gen_random_string(depth); break;
            case G_CSTR_T:     break;
            case G_INT_T:      g = G_INT(ugeneric_random_from_range(INT_MIN, RAND_MAX)); break;
            case G_REAL_T:     g = G_REAL(ugeneric_random_from_range(INT_MIN , RAND_MAX)); break;
            case G_SIZE_T:     g = G_SIZE(ugeneric_random_from_range(0, RAND_MAX)); break;
            case G_BOOL_T:     g = G_BOOL(ugeneric_random_from_range(0, 2)); break;
            case G_VECTOR_T:   g = gen_random_vector(depth - 1); break;
            case G_DICT_T:     g =  gen_random_dict(depth - 1); break;
            case G_MEMCHUNK_T: g = gen_random_memchunk(depth - 1); break;
                default:
                    ;
        }
    }

    return g;
}

ugeneric_t gen_random_vector(int depth)
{
    ugeneric_t g = G_CSTR("v");
    if (depth > 1)
    {
        uvector_t *v = uvector_create();
        int size = ugeneric_random_from_range(1, 100);
        printf("[%02d] Generate vector of size %d\n", depth, size);
        for (int i = 0; i < size; i++)
        {
            uvector_append(v, gen_random_generic(depth - 1));
        }

        uvector_print(v);
        g = G_VECTOR(v);
    }

    return g;
}

ugeneric_t gen_random_string(int depth)
{
    int size = ugeneric_random_from_range(1, 20);
    //int size = ugeneric_random_from_range(0, 20);
    char *str = umalloc(size + 1);
    for (int i = 0; i < size; i++)
    {
        str[i] = ugeneric_random_from_range('a', 'z');
    }
    str[size] = 0;
    printf("[%02d] Generated string '%s'\n", depth, str);

    return G_STR(str);
}

ugeneric_t gen_random_dict(int depth)
{
    // TODO: random backends
    udict_t *d = udict_create();

    //int size = ugeneric_random_from_range(0, 20);
    int size = ugeneric_random_from_range(1, 20);
    printf("[%02d] Generate dict of size %d\n", depth, size);
    for (int i = 0; i < size; i++)
    {
        ugeneric_t k = gen_random_string(depth);
        //ugeneric_t v = gen_random_string();
        //ugeneric_t k = gen_random_generic(depth - 1);
        ugeneric_t v = gen_random_generic(depth - 1);
        udict_put(d, k, v);
    }

    return G_DICT(d);
}

ugeneric_t gen_random_memchunk(int depth)
{
    int size = ugeneric_random_from_range(1, 50);
    void *ptr = umalloc(size);
    printf("[%02d] Generate memchunk of size %d\n", depth, size);
    for (int i = 0; i < size; i++)
    {
        ((uint8_t *)ptr)[i] = ugeneric_random_from_range(0, 255);
    }
    return G_MEMCHUNK(ptr, size);
}
