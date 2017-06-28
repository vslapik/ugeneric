#include <limits.h>
#include "ut_utils.h"

ugeneric_t gen_random_generic(int depth, bool verbose, bool exclude_non_hashable)
{
    ugeneric_t g = G_CSTR("g");
    if (depth > 0)
    {
        ugeneric_type_e gtype;
        do {
            gtype = ugeneric_random_from_range(G_NULL_T, G_MEMCHUNK_T);
            UASSERT(gtype > G_ERROR_T);
            UASSERT(gtype <= G_MEMCHUNK_T);
        } while (exclude_non_hashable && ((gtype == G_VECTOR_T) || (gtype == G_DICT_T)));

        switch(gtype)
        {
            case G_NULL_T:     g = G_NULL; break;
            case G_PTR_T:      g = gen_random_void_data(depth - 1, verbose); break;
            case G_STR_T:      g = gen_random_string(depth, verbose); break;
            case G_CSTR_T:     break;
            case G_INT_T:      g = G_INT(ugeneric_random_from_range(INT_MIN, RAND_MAX)); break;
            case G_REAL_T:     g = G_REAL(ugeneric_random_from_range(INT_MIN , RAND_MAX)); break;
            case G_SIZE_T:     g = G_SIZE(ugeneric_random_from_range(0, RAND_MAX)); break;
            case G_BOOL_T:     g = G_BOOL(ugeneric_random_from_range(0, 2)); break;
            case G_VECTOR_T:   g = gen_random_vector(depth - 1, verbose); break;
            case G_DICT_T:     g = gen_random_dict(depth - 1, verbose); break;
            case G_MEMCHUNK_T: g = gen_random_memchunk(depth - 1, verbose); break;
            default: ;
        }
    }

    return g;
}

int _void_cmp(const void *ptr1, const void *ptr2)
{
    return (uintptr_t)ptr1 - (uintptr_t)ptr2;
}

size_t _void_hash(const void *ptr)
{
    return (size_t)ptr;
}

void *_void_cpy(const void *ptr)
{
    return (void*)ptr;
}

ugeneric_t gen_random_vector(int depth, bool verbose)
{
    ugeneric_t g = G_CSTR("v");
    if (depth > 1)
    {
        uvector_t *v = uvector_create();
        uvector_set_destroyer(v, ufree);
        uvector_set_comparator(v, _void_cmp);
        uvector_set_copier(v, _void_cpy);
        int size = ugeneric_random_from_range(0, 100);
        if (verbose)
        {
            printf("[%02d] Generate vector of size %d\n", depth, size);
        }
        for (int i = 0; i < size; i++)
        {
            uvector_append(v, gen_random_generic(depth - 1, verbose, false));
        }
        uvector_shrink_to_size(v);

        if (verbose)
        {
            uvector_print(v);
        }
        g = G_VECTOR(v);
    }

    return g;
}

ugeneric_t gen_random_string(int depth, bool verbose)
{
    int size = ugeneric_random_from_range(0, 20);
    char *str = umalloc(size + 1);
    for (int i = 0; i < size; i++)
    {
        str[i] = ugeneric_random_from_range('a', 'z');
    }
    str[size] = 0;
    if (verbose)
    {
        printf("[%02d] Generated string '%s'\n", depth, str);
    }

    return G_STR(str);
}

ugeneric_t gen_random_dict(int depth, bool verbose)
{
    udict_backend_t backend = ugeneric_random_from_range(UDICT_BACKEND_DEFAULT + 1, UDICT_BACKENDS_COUNT - 1);
    udict_t *d = udict_create_with_backend(backend);
    udict_set_destroyer(d, ufree);
    udict_set_comparator(d, _void_cmp);
    if (backend == UDICT_BACKEND_HTBL)
    {
        udict_set_hasher(d, _void_hash);
        udict_set_key_comparator(d, _void_cmp);
    }

    int size = ugeneric_random_from_range(0, 20);
    if (verbose)
    {
        printf("[%02d] Generate dict of size %d\n", depth, size);
    }
    for (int i = 0; i < size; i++)
    {
        ugeneric_t k = gen_random_generic(depth - 1, verbose, true); // keys must be hashable
        ugeneric_t v = gen_random_generic(depth - 1, verbose, false);
        udict_put(d, k, v);
    }

    return G_DICT(d);
}

ugeneric_t gen_random_memchunk(int depth, bool verbose)
{
    int size = ugeneric_random_from_range(1, 50);
    void *ptr = umalloc(size);
    if (verbose)
    {
        printf("[%02d] Generate memchunk of size %d\n", depth, size);
    }
    for (int i = 0; i < size; i++)
    {
        ((uint8_t *)ptr)[i] = ugeneric_random_from_range(0, 255);
    }
    return G_MEMCHUNK(ptr, size);
}

ugeneric_t gen_random_void_data(int depth, bool verbose)
{
    ugeneric_t gm = gen_random_memchunk(depth - 1, verbose);
    return G_PTR(G_AS_MEMCHUNK_DATA(gm));
}
