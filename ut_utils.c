#include "ut_utils.h"

#include "heap.h"
#include "string_utils.h"

#include <limits.h>

typedef struct {
    int i1;
    int i2;
    int i3;
} void_t;

ugeneric_t gen_random_generic(int depth, size_t ctn_max_len, bool verbose, bool exclude_containers)
{
    UASSERT(depth >= 0);

    // G_VECTOR_T and G_DICT_T are the last two, optionally can be excluded
    int types[] = {G_NULL_T, /*G_PTR_T, G_CPTR,*/ G_STR_T, G_CSTR_T, G_INT_T,
                   G_REAL_T, G_SIZE_T, G_BOOL_T, G_MEMCHUNK_T, G_VECTOR_T, G_DICT_T};
    if (!depth)
    {
        exclude_containers = true;
    }
    ugeneric_type_e gtype = types[
        ugeneric_random_from_range(0, ARRAY_LEN(types) - 2 * exclude_containers - 1)
    ];

    ugeneric_t g;
    switch (gtype)
    {
        case G_NULL_T:     g = G_NULL();                                            break;
//            case G_PTR_T:      g = gen_random_void_data(depth, verbose);              break;
        case G_STR_T:      g = gen_random_string(depth, verbose);                   break;
        case G_CSTR_T:     g = gen_random_string(depth, verbose);                   break;
        case G_INT_T:      g = G_INT(ugeneric_random_from_range(-100, 100));        break;
        case G_REAL_T:     g = G_REAL(ugeneric_random_from_range(100, 200)/1000.0); break;
        case G_SIZE_T:     g = G_SIZE(ugeneric_random_from_range(0, RAND_MAX - 1)); break;
        case G_BOOL_T:     g = G_BOOL(ugeneric_random_from_range(0, 1));            break;
        case G_VECTOR_T:   g = gen_random_vector(depth, ctn_max_len, verbose);      break;
        case G_DICT_T:     g = gen_random_dict(depth, ctn_max_len, verbose);        break;
        case G_MEMCHUNK_T: g = gen_random_memchunk(depth, verbose);                 break;
        default:
            UABORT("internal error");
    }

    return g;
}

static int _void_cmp(const void *ptr1, const void *ptr2)
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

ugeneric_t gen_random_vector(int depth, size_t ctn_max_len, bool verbose)
{
    UASSERT(depth);

    uvector_t *v = uvector_create();
    uvector_set_void_destroyer(v, _void_dtr);
    uvector_set_void_comparator(v, _void_cmp);
    uvector_set_void_copier(v, _void_cpy);
    uvector_set_void_serializer(v, _void_s8r);
    int size = ugeneric_random_from_range(0, ctn_max_len);

    for (int i = 0; i < size; i++)
    {
        uvector_append(v, gen_random_generic(depth - 1, ctn_max_len, verbose, false));
    }
    uvector_shrink_to_size(v);

    if (verbose)
    {
        printf("[%02d] Generate vector of size %d\n", depth, size);
        uvector_print(v);
    }

    return G_VECTOR(v);
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

ugeneric_t gen_random_dict(int depth, size_t ctn_max_len, bool verbose)
{
    UASSERT(depth);

    udict_backend_t backend = ugeneric_random_from_range(UDICT_BACKEND_DEFAULT + 1, UDICT_BACKEND_MAX - 1);
    udict_t *d = udict_create_with_backend(backend);
    udict_set_void_destroyer(d, ufree);
    udict_set_void_comparator(d, _void_cmp);
    udict_set_void_copier(d,     _void_cpy);
    udict_set_void_serializer(d, _void_s8r);
    if (UDICT_ON_HTBL(d))
    {
        udict_set_void_hasher(d, _void_hash);
        udict_set_void_key_comparator(d, _void_cmp);
    }

    int size = ugeneric_random_from_range(0, 50);
    for (int i = 0; i < size; i++)
    {
        ugeneric_t k = gen_random_generic(depth - 1, ctn_max_len, verbose, true); // keys must be hashable
        ugeneric_t v = gen_random_generic(depth - 1, ctn_max_len, verbose, false);
        udict_put(d, k, v);
    }

    if (verbose)
    {
        printf("[%02d] Generated dict of size %d\n", depth, size);
    }

    return G_DICT(d);
}

ugeneric_t gen_random_memchunk(int depth, bool verbose)
{
    int size = ugeneric_random_from_range(1, 50);
    void *ptr = umalloc(size);
    for (int i = 0; i < size; i++)
    {
        ((uint8_t *)ptr)[i] = ugeneric_random_from_range(0, 255);
    }
    if (verbose)
    {
        printf("[%02d] Generated memchunk of size %d\n", depth, size);
    }

    return G_MEMCHUNK(ptr, size);
}

ugeneric_t gen_random_void_data(int depth, bool verbose)
{
    void_t *p = umalloc(sizeof(*p));
    p->i1 = ugeneric_random_from_range(0, 500);
    p->i2 = ugeneric_random_from_range(0, 500);
    p->i3 = ugeneric_random_from_range(0, 500);

    if (verbose)
    {
        printf("[%02d] Generated void data\n", depth);
    }

    return G_PTR(p);
}
