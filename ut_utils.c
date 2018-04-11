#include <limits.h>
#include "ut_utils.h"
#include "void.h"
#include "string_utils.c"

typedef struct {
    int i1;
    int i2;
    int i3;
} void_t;

ugeneric_t gen_random_generic(int depth, bool verbose, bool exclude_non_hashable)
{
    if (depth > 0)
    {
        ugeneric_t g = G_CSTR("g");
        ugeneric_type_e gtype;
        do {
            gtype = ugeneric_random_from_range(G_NULL_T, G_MEMCHUNK_T);
            UASSERT(gtype > G_ERROR_T);
            UASSERT(gtype <= G_MEMCHUNK_T);
        } while (exclude_non_hashable && ((gtype == G_VECTOR_T) || (gtype == G_DICT_T)));

        switch(gtype)
        {
            case G_NULL_T:     g = G_NULL(); break;
            case G_PTR_T:      g = gen_random_void_data(depth - 1, verbose); break;
            case G_STR_T:      g = gen_random_string(depth, verbose); break;
            case G_CSTR_T:     break;
            case G_INT_T:      g = G_INT(ugeneric_random_from_range(-100, 100)); break;
            case G_REAL_T:     g = G_REAL(ugeneric_random_from_range(-1000 , 1000)/10.0); break;
            case G_SIZE_T:     g = G_SIZE(ugeneric_random_from_range(200, 300)); break;
            case G_BOOL_T:     g = G_BOOL(ugeneric_random_from_range(0, 2)); break;
            case G_VECTOR_T:   g = gen_random_vector(depth - 1, verbose); break;
            case G_DICT_T:     g = gen_random_dict(depth - 1, verbose); break;
            case G_MEMCHUNK_T: g = gen_random_memchunk(depth - 1, verbose); break;
            default: ;
        }

        return g;
    }
    else
    {
        return G_STR(ustring_fmt("%d", depth));
    }
}

int _void_cmp(const void *ptr1, const void *ptr2)
{
    int ret = 0;
    const void_t *p1 = ptr1;
    const void_t *p2 = ptr2;

    ret = p1->i1 - p2->i1;
    if (ret == 0)
    {
        ret = p1->i2 - p2->i2;
        if (ret == 0)
        {
            ret = p1->i3 - p2->i3;
        }
    }

    return ret;
}

size_t _void_hash(const void *ptr)
{
    const void_t *p = ptr;
    return p->i1 + p->i2 + p->i3;
}

void *_void_cpy(const void *ptr)
{
    const void_t *p1 = ptr;
    void_t *p2 = umalloc(sizeof(*p1));
    memcpy(p2, p1, sizeof(void_t));

    return p2;
}

void _void_dtr(void *ptr)
{
    ufree(ptr);
}

char *_void_s8r(const void *ptr, size_t *output_size)
{
    const void_t *p = ptr;
    return ustring_fmt_sized("\"%d-%d-%d\"", output_size, p->i1, p->i2, p->i3);
}

ugeneric_t gen_random_vector(int depth, bool verbose)
{
    ugeneric_t g = G_CSTR("v");
    if (depth > 1)
    {
        uvector_t *v = uvector_create();
        uvector_set_void_destroyer(v, _void_dtr);
        uvector_set_void_comparator(v, _void_cmp);
        uvector_set_void_copier(v, _void_cpy);
        uvector_set_void_serializer(v, _void_s8r);
        int size = ugeneric_random_from_range(0, 50);
        if (verbose)
        {
            printf("[%02d] Generate vector of size %d\n", depth, size);
        }
        for (int i = 0; i < size; i++)
        {
            uvector_append(v, gen_random_generic(depth - 1, verbose, false));
        }
        uvector_shrink_to_size(v);
/*
        if (size > 1)
        {
            size_t i = ugeneric_random_from_range(0, size - 1);
            ugeneric_t e = uvector_pop_at(v, i);
            size_t j = ugeneric_random_from_range(0, size - 2);
            uvector_insert_at(v, j, e);
            size_t k = ugeneric_random_from_range(0, size - 1);
            uvector_remove_at(v, k);
        }
        */

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
    udict_set_void_destroyer(d, ufree);
    udict_set_void_comparator(d, _void_cmp);
    udict_set_void_copier(d, _void_cpy);
    udict_set_void_serializer(d, _void_s8r);
    if (backend == UDICT_BACKEND_HTBL)
    {
        udict_set_void_hasher(d, _void_hash);
        udict_set_void_key_comparator(d, _void_cmp);
    }

    int size = ugeneric_random_from_range(0, 50);
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
    (void)depth;
    (void)verbose;

    void_t *p = umalloc(sizeof(*p));
    p->i1 = ugeneric_random_from_range(0, 500);
    p->i2 = ugeneric_random_from_range(0, 500);
    p->i3 = ugeneric_random_from_range(0, 500);

    return G_PTR(p);
}
