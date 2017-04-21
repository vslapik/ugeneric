#include "heap.h"
#include "vector.h"
#include "mem.h"

#define HEAP_INITIAL_CAPACITY 16

#define PARENT_IDX(i) (((i) - 1) / 2)
#define LCHILD_IDX(i) (2 * (i) + 1)
#define RCHILD_IDX(i) (2 * (i) + 2)
#define ROOT_IDX 0

struct heap_opaq {
    vector_t *data;
    void_cmp_t cmp;
};

heap_t *heap_create(void)
{
    return heap_create_with_capacity(HEAP_INITIAL_CAPACITY);
}

heap_t *heap_create_with_capacity(size_t capacity)
{
    heap_t *h = umalloc(sizeof(*h));
    h->data = NULL;
    h->data = vector_create();
    vector_reserve_capacity(h->data, capacity);

    return h;
}

void heap_destroy(heap_t *h)
{
    if (h)
    {
        vector_destroy(h->data);
        ufree(h);
    }
}

void heap_clear(heap_t *h)
{
    ASSERT_INPUT(h);
    vector_clear(h->data);
}

void heap_push(heap_t *h, generic_t e)
{
    ASSERT_INPUT(h);

    vector_append(h->data, e);
    generic_t *a = vector_get_cells(h->data);
    size_t i = vector_get_size(h->data) - 1;

    while (i != ROOT_IDX)
    {
        if (generic_compare(a[i], a[PARENT_IDX(i)], h->cmp) < 0)
        {
            vector_swap(h->data, i, PARENT_IDX(i));
            i = PARENT_IDX(i);
        }
        else
        {
            break;
        }
    }
}

generic_t heap_pop(heap_t *h)
{
    ASSERT_INPUT(h);
    ASSERT_INPUT(!vector_is_empty(h->data));

    generic_t e = vector_get_at(h->data, 0);
    generic_t e1 = vector_pop_back(h->data);
    size_t n = vector_get_size(h->data);

    if (n)
    {
        size_t i = 0;
        size_t t = 0;
        size_t l = LCHILD_IDX(i);
        size_t r = RCHILD_IDX(i);
        vector_set_at(h->data, 0, e1);
        generic_t *a = vector_get_cells(h->data);

        while (l < n || r < n)
        {
            if (r < n)
            {
                t = LCHILD_IDX(i);
                if (generic_compare(a[l], a[r], h->cmp) > 0)
                {
                    t = RCHILD_IDX(i);
                }
            }
            if (generic_compare(a[i], a[t], h->cmp) > 0)
            {
                vector_swap(h->data, i, t);
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

generic_t heap_peek(const heap_t *h)
{
    ASSERT_INPUT(h);
    ASSERT_INPUT(!vector_is_empty(h->data));
    return vector_get_at(h->data, 0);
}

size_t heap_get_size(const heap_t *h)
{
    ASSERT_INPUT(h);
    return vector_get_size(h->data);
}

bool heap_is_empty(const heap_t *h)
{
    ASSERT_INPUT(h);
    return (vector_get_size(h->data) == 0);
}
