#include <stddef.h>
#include <inttypes.h>
#include "heap.h"
#include "vector.h"
#include "mem.h"
#include "string_utils.h"

#define HEAP_INITIAL_CAPACITY 16

#define PARENT_IDX(i) (((i) - 1) / 2)
#define LCHILD_IDX(i) (2 * (i) + 1)
#define RCHILD_IDX(i) (2 * (i) + 2)
#define ROOT_IDX 0

struct heap_opaq {
    uvector_t *data;
    heap_type_t type;
};

heap_t *heap_create(void)
{
    return heap_create_ext(HEAP_INITIAL_CAPACITY, MIN_HEAP);
}

heap_t *heap_create_ext(size_t capacity, heap_type_t type)
{
    UASSERT_INPUT((type == MIN_HEAP) || (type == MAX_HEAP));
    heap_t *h = umalloc(sizeof(*h));
    h->data = NULL;
    h->data = uvector_create();
    uvector_reserve_capacity(h->data, capacity);
    h->type = type;

    return h;
}

void heap_destroy(heap_t *h)
{
    if (h)
    {
        uvector_destroy(h->data);
        ufree(h);
    }
}

void heap_clear(heap_t *h)
{
    UASSERT_INPUT(h);
    uvector_clear(h->data);
}

void heap_push(heap_t *h, ugeneric_t e)
{
    UASSERT_INPUT(h);

    uvector_append(h->data, e);
    ugeneric_t *a = uvector_get_cells(h->data);
    size_t i = uvector_get_size(h->data) - 1;

    void_cmp_t cmp = uvector_get_comparator(h->data);
    while (i != ROOT_IDX)
    {
        if (h->type * ugeneric_compare(a[i], a[PARENT_IDX(i)], cmp) < 0)
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

ugeneric_t heap_pop(heap_t *h)
{
    UASSERT_INPUT(h);
    UASSERT_INPUT(!uvector_is_empty(h->data));

    ugeneric_t *a = uvector_get_cells(h->data);
    ugeneric_t e = a[0]; // take the root
    ugeneric_t e1 = uvector_pop_back(h->data); // take the last element
    size_t n = uvector_get_size(h->data);

    if (n)
    {
        size_t i = 0;
        size_t t = 0;
        size_t l = LCHILD_IDX(i);
        size_t r = RCHILD_IDX(i);
        a[0] = e1; // copy the last element to the root

        void_cmp_t cmp = uvector_get_comparator(h->data);
        while (l < n || r < n) // percolate the root down to the right position
        {
            if (r < n)
            {
                t = LCHILD_IDX(i);
                if (h->type * ugeneric_compare(a[l], a[r], cmp) > 0)
                {
                    t = RCHILD_IDX(i);
                }
            }
            else if (l < n)
            {
                t = LCHILD_IDX(i);
            }
            if (h->type * ugeneric_compare(a[i], a[t], cmp) > 0)
            {
                ugeneric_swap(&a[i], &a[t]);
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

    return e;
}

ugeneric_t heap_peek(const heap_t *h)
{
    UASSERT_INPUT(h);
    UASSERT_INPUT(!uvector_is_empty(h->data));
    return uvector_get_at(h->data, 0);
}

size_t heap_get_size(const heap_t *h)
{
    UASSERT_INPUT(h);
    return uvector_get_size(h->data);
}

bool heap_is_empty(const heap_t *h)
{
    UASSERT_INPUT(h);
    return (uvector_get_size(h->data) == 0);
}

size_t heap_get_capacity(const heap_t *h)
{
    UASSERT_INPUT(h);
    return uvector_get_capacity(h->data);
}

void heap_reserve_capacity(heap_t *h, size_t new_capacity)
{
    UASSERT_INPUT(h);
    uvector_reserve_capacity(h->data, new_capacity);
}

void heap_take_data_ownership(heap_t *h)
{
    UASSERT_INPUT(h);
    uvector_take_data_ownership(h->data);
}

void heap_drop_data_ownership(heap_t *h)
{
    UASSERT_INPUT(h);
    uvector_drop_data_ownership(h->data);
}

void heap_set_destroyer(heap_t *h, void_dtr_t dtr)
{
    UASSERT_INPUT(h);
    uvector_set_destroyer(h->data, dtr);
}

void heap_set_comparator(heap_t *h, void_cmp_t cmp)
{
    UASSERT_INPUT(h);
    uvector_set_comparator(h->data, cmp);
}

void heap_set_copier(heap_t *h, void_cpy_t cpy)
{
    UASSERT_INPUT(h);
    uvector_set_copier(h->data, cpy);
}

void_dtr_t heap_get_destroyer(const heap_t *h)
{
    UASSERT_INPUT(h);
    return uvector_get_destroyer(h->data);
}

void_cmp_t heap_get_comparator(const heap_t *h)
{
    UASSERT_INPUT(h);
    return uvector_get_comparator(h->data);
}

void_cpy_t heap_get_copier(const heap_t *h)
{
    UASSERT_INPUT(h);
    return uvector_get_copier(h->data);
}

ugeneric_t *heap_get_cells(const heap_t *h)
{
    UASSERT_INPUT(h);
    return uvector_get_cells(h->data);
}

void heap_dump_to_dot(const heap_t *h, const char *name, FILE *out)
{
    UASSERT_INPUT(h);

    fprintf(out, "digraph %s {\n", name);
    fprintf(out, "    label=\"%s\";\n", name);
    fprintf(out, "    labelloc=top;\n");

    ugeneric_t *a = uvector_get_cells(h->data);
    size_t size = uvector_get_size(h->data);
    for (size_t i = 0; i < size; i++)
    {
        char *str =  ustring_fmt("\"%08" PRIxPTR "\"", &a[i]);
        char *lstr = LCHILD_IDX(i) < size ? ustring_fmt("\"%08" PRIxPTR "\"", &a[LCHILD_IDX(i)]) : NULL;
        char *rstr = RCHILD_IDX(i) < size ? ustring_fmt("\"%08" PRIxPTR "\"", &a[RCHILD_IDX(i)]) : NULL;
        char *vstr = ugeneric_as_str(a[i], NULL);

        fprintf(out, "    %s [style = filled, fillcolor = %s, fontcolor = %s, label = %s];\n",
                str,
                "black",
                "white",
                vstr);
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
