#include "heap.h"

#include "asserts.h"
#include "mem.h"
#include "string_utils.h"
#include "vector.h"
#include <inttypes.h>

#define HEAP_INITIAL_CAPACITY 16

#define PARENT_IDX(i) (((i) - 1) / 2)
#define LCHILD_IDX(i) (2 * (i) + 1)
#define RCHILD_IDX(i) (2 * (i) + 2)
#define ROOT_IDX 0

struct uheap_opaq {
    uvector_t *data;
    uheap_type_t type;
};

static uheap_t *_hcpy(const uheap_t *h, bool deep)
{
    uheap_t *copy = umalloc(sizeof(*copy));
    *copy = *h;
    copy->data = deep ? uvector_deep_copy(h->data) : uvector_copy(h->data);
    return copy;
}

static void _heapify(ugeneric_t *base, size_t nmemb, size_t i,
                     uheap_type_t type, void_cmp_t cmp)
{
    size_t t = 0;
    size_t l = LCHILD_IDX(i);
    size_t r = RCHILD_IDX(i);

    while (l < nmemb || r < nmemb) // percolate i-th element down to the correct position
    {
        if (r < nmemb)
        {
            t = LCHILD_IDX(i);
            if (type * ugeneric_compare_v(base[l], base[r], cmp) > 0)
            {
                t = RCHILD_IDX(i);
            }
        }
        else if (l < nmemb)
        {
            t = LCHILD_IDX(i);
        }
        if (type * ugeneric_compare_v(base[i], base[t], cmp) > 0)
        {
            ugeneric_swap(&base[i], &base[t]);
            i = t;
            l = LCHILD_IDX(i);
            r = RCHILD_IDX(i);
        }
        else
        {
            break;
        }
    }
}

uheap_t *uheap_build_from_array(const ugeneric_t *base, size_t nmemb,
                                uheap_type_t type,
                                uvoid_handlers_t *void_handlers)
{
    uheap_t *h = uheap_create_ext(nmemb, type);

    if (void_handlers)
    {
        uheap_get_base(h)->void_handlers = *void_handlers;
    }

    uvector_resize(h->data, nmemb, G_NULL());
    ugeneric_t *a = uvector_get_cells(h->data);

    // Deep copy input array to the heap.
    for (size_t i = 0; i < nmemb; i++)
    {
        a[i] = ugeneric_copy_v(base[i], uheap_get_void_copier(h));
    }

    size_t i = nmemb / 2;
    while (true)
    {
        _heapify(a, nmemb, i, type, uheap_get_void_comparator(h));
        if (i == 0)
        {
            break;
        }
        else
        {
            i--;
        }
    }

    return h;
}

uheap_t *uheap_create(void)
{
    return uheap_create_ext(HEAP_INITIAL_CAPACITY, UHEAP_TYPE_MIN);
}

uheap_t *uheap_create_ext(size_t capacity, uheap_type_t type)
{
    UASSERT_INPUT((type == UHEAP_TYPE_MIN) || (type == UHEAP_TYPE_MAX));
    uheap_t *h = umalloc(sizeof(*h));
    h->data = uvector_create();
    uvector_reserve_capacity(h->data, capacity);
    h->type = type;

    return h;
}

void uheap_destroy(uheap_t *h)
{
    if (h)
    {
        uvector_destroy(h->data);
        ufree(h);
    }
}

void uheap_clear(uheap_t *h)
{
    UASSERT_INPUT(h);
    uvector_clear(h->data);
}

void uheap_push(uheap_t *h, ugeneric_t e)
{
    UASSERT_INPUT(h);

    uvector_append(h->data, e);
    ugeneric_t *a = uvector_get_cells(h->data);
    size_t i = uvector_get_size(h->data) - 1;

    void_cmp_t cmp = uheap_get_void_comparator(h);
    while (i != ROOT_IDX)
    {
        // Bubble up the new element to its proper position.
        if (h->type * ugeneric_compare_v(a[i], a[PARENT_IDX(i)], cmp) < 0)
        {
            ugeneric_swap(&a[i], &a[PARENT_IDX(i)]);
            i = PARENT_IDX(i);
        }
        else
        {
            break;
        }
    }
}

ugeneric_t uheap_pop(uheap_t *h)
{
    UASSERT_INPUT(h);
    UASSERT_INPUT(!uvector_is_empty(h->data));

    ugeneric_t *a = uvector_get_cells(h->data);
    ugeneric_t e = a[0]; // take the root
    ugeneric_t e1 = uvector_pop_back(h->data); // take the last element
    size_t n = uvector_get_size(h->data);

    if (n)
    {
        // move the last element to the root
        a[0] = e1;
        // percolate the root down to the right position
        _heapify(a, n, 0, h->type, uheap_get_void_comparator(h));
    }

    return e;
}

ugeneric_t uheap_peek(const uheap_t *h)
{
    UASSERT_INPUT(h);
    UASSERT_INPUT(!uvector_is_empty(h->data));
    return uvector_get_at(h->data, 0);
}

size_t uheap_get_size(const uheap_t *h)
{
    UASSERT_INPUT(h);
    return uvector_get_size(h->data);
}

bool uheap_is_empty(const uheap_t *h)
{
    UASSERT_INPUT(h);
    return (uvector_get_size(h->data) == 0);
}

uheap_type_t uheap_get_type(const uheap_t *h)
{
    UASSERT_INPUT(h);
    return h->type;
}

size_t uheap_get_capacity(const uheap_t *h)
{
    UASSERT_INPUT(h);
    return uvector_get_capacity(h->data);
}

void uheap_reserve_capacity(uheap_t *h, size_t new_capacity)
{
    UASSERT_INPUT(h);
    uvector_reserve_capacity(h->data, new_capacity);
}

ugeneric_t *uheap_get_cells(const uheap_t *h)
{
    UASSERT_INPUT(h);
    return uvector_get_cells(h->data);
}

uheap_t *uheap_copy(const uheap_t *h)
{
    UASSERT_INPUT(h);
    return _hcpy(h, false);
}

uheap_t *uheap_deep_copy(const uheap_t *h)
{
    UASSERT_INPUT(h);
    return _hcpy(h, true);
}

void uheap_dump_to_dot(const uheap_t *h, const char *name, FILE *out)
{
    UASSERT_INPUT(h);

    fprintf(out, "digraph %s {\n", name);
    fprintf(out, "    label=\"%s\";\n", name);
    fprintf(out, "    labelloc=top;\n");

    ugeneric_t *a = uvector_get_cells(h->data);
    size_t size = uvector_get_size(h->data);
    for (size_t i = 0; i < size; i++)
    {
        char *str  = ustring_fmt("\"%08" PRIxPTR "\"", &a[i]);
        char *lstr = LCHILD_IDX(i) < size ? ustring_fmt("\"%08" PRIxPTR "\"", &a[LCHILD_IDX(i)]) : NULL;
        char *rstr = RCHILD_IDX(i) < size ? ustring_fmt("\"%08" PRIxPTR "\"", &a[RCHILD_IDX(i)]) : NULL;
        char *vstr = ugeneric_as_str(a[i]);

        fprintf(out, "    %s [style = filled, fillcolor = %s, fontcolor = %s, label = %s];\n",
                str, "black", "white", vstr);
        if (lstr)
        {
            fprintf(out, "    %s -> %s;\n", str, lstr);
        }
        if (rstr)
        {
            fprintf(out, "    %s -> %s;\n", str, rstr);
        }
        ufree(str); ufree(lstr); ufree(rstr); ufree(vstr);
    }
    fprintf(out, "}\n");
}

ugeneric_base_t *uheap_get_base(uheap_t *h)
{
    UASSERT_INPUT(h);
    return uvector_get_base(h->data);
}

char *uheap_as_str(const uheap_t *h)
{
    return uvector_as_str(h->data);
}

void uheap_serialize(const uheap_t *h, ubuffer_t *buf)
{
    uvector_serialize(h->data, buf);
}

int uheap_fprint(const uheap_t *h, FILE *out)
{
    return uvector_fprint(h->data, out);
}
