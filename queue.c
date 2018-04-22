#include <stdlib.h>
#include "generic.h"
#include "mem.h"
#include "queue.h"

/* out <-- [h][e][e][...][e][e][t] <-- in */

struct uqueue_opaq {
    uvoid_handlers_t void_handlers;
    bool is_data_owner;
    ugeneric_t *data;
    size_t h; // head
    size_t t; // tail
    size_t size;
    size_t capacity;
};

uqueue_t *uqueue_create(void)
{
    uqueue_t *q = umalloc(sizeof(*q));
    q->data = NULL;
    q->h = 0;
    q->t = 0;
    q->size = 0;
    q->capacity = 0;
    memset(&q->void_handlers, 0, sizeof(q->void_handlers));

    return q;
}

void uqueue_destroy(uqueue_t *q)
{
    if (q)
    {
        ufree(q->data);
        ufree(q);
    }
}

void uqueue_clear(uqueue_t *q)
{
    UASSERT_INPUT(q);

    q->h = 0;
    q->t = 0;
    q->size = 0;
}

void uqueue_enq(uqueue_t *q, ugeneric_t element)
{
    UASSERT_INPUT(q);

    if (q->capacity == q->size)
    {
        size_t new_capacity = MAX(SCALE_FACTOR * q->size,
                                  QUEUE_INITIAL_CAPACITY);
        uqueue_reserve_capacity(q, new_capacity ? new_capacity : 1);
    }
    if (q->size)
    {
        q->t = (q->t + 1) % q->capacity;
    }
    q->data[q->t] = element;
    q->size++;
}

ugeneric_t uqueue_deq(uqueue_t *q)
{
    UASSERT_INPUT(q);
    UASSERT_MSG(q->size, "dequeuing from an empty queue");

    ugeneric_t e = q->data[q->h];
    if (q->h != q->t)
    {
        q->h = (q->h + 1) % q->capacity;
    }
    q->size--;

    return e;
}

ugeneric_t uqueue_peek(const uqueue_t *q)
{
    UASSERT_INPUT(q);
    UASSERT_MSG(q->size, "peeking from an empty queue");
    return q->data[q->h];
}

size_t uqueue_get_size(const uqueue_t *q)
{
    UASSERT_INPUT(q);
    return q->size;
}

size_t uqueue_get_capacity(const uqueue_t *q)
{
    UASSERT_INPUT(q);
    return q->capacity;
}

bool uqueue_is_empty(const uqueue_t *q)
{
    UASSERT_INPUT(q);
    return q->size == 0;
}

void uqueue_reserve_capacity(uqueue_t *q, size_t new_capacity)
{
    UASSERT_INPUT(q);

    if (q->capacity < new_capacity)
    {
        /* If queue is not empty and we need more slots we allocate
         * additional memory and move existing elements to bigger room.
         * Old queue memory should be freed.
         */
        if (q->size)
        {
            ugeneric_t *p = umalloc(new_capacity * sizeof(*p));
            for (size_t i = 0; i < q->size; i++)
            {
                p[i] = q->data[(q->h + i) % q->capacity];
            }
            ufree(q->data);
            q->data = p;
            q->h = 0;
            q->t = q->size - 1;
            q->capacity = new_capacity;
        }
        else
        {
            q->data = umalloc(new_capacity * sizeof(q->data[0]));
            q->capacity = new_capacity;
        }
    }
}

void uqueue_serialize(const uqueue_t *q, ubuffer_t *buf)
{
    UASSERT_INPUT(q);
    UASSERT_INPUT(buf);

    ubuffer_append_byte(buf, '[');
    for (size_t i = 0; i < q->size; i++)
    {
        ugeneric_serialize_v(q->data[(q->h + i) % q->capacity], buf, q->void_handlers.s8r);
        if (i < q->size - 1)
        {
            ubuffer_append_data(buf, ", ", 2);
        }
    }
    ubuffer_append_byte(buf, ']');
}

char *uqueue_as_str(const uqueue_t *q)
{
    UASSERT_INPUT(q);

    ubuffer_t buf = {0};
    uqueue_serialize(q, &buf);
    ubuffer_null_terminate(&buf);

    return buf.data;
}

int uqueue_fprint(const uqueue_t *q, FILE *out)
{
    UASSERT_INPUT(q);
    UASSERT_INPUT(out);

    char *str = uqueue_as_str(q);
    int ret = fprintf(out, "%s\n", str);
    ufree(str);

    return ret;
}

ugeneric_base_t *uqueue_get_base(uqueue_t *q)
{
    UASSERT_INPUT(q);
    return (ugeneric_base_t *)q;
}
