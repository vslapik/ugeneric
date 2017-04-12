#include <stdlib.h>
#include "generic.h"
#include "mem.h"
#include "queue.h"

/* out <-- [h][e][e][...][e][e][t] <-- in */

struct queue_opaq {
    generic_t *data;
    size_t h; // head
    size_t t; // tail
    size_t size;
    size_t capacity;
};

queue_t *queue_create(void)
{
    queue_t *q = umalloc(sizeof(*q));
    q->data = NULL;
    q->h = 0;
    q->t = 0;
    q->size = 0;
    q->capacity = 0;

    return q;
}

void queue_destroy(queue_t *q)
{
    if (q)
    {
        ufree(q->data);
        ufree(q);
    }
}

void queue_clear(queue_t *q)
{
    ASSERT_INPUT(q);

    q->h = 0;
    q->t = 0;
    q->size = 0;
}

void queue_enq(queue_t *q, generic_t element)
{
    ASSERT_INPUT(q);

    if (q->capacity == q->size)
    {
        size_t new_capacity = MAX(SCALE_FACTOR * q->size,
                                  QUEUE_INITIAL_CAPACITY);
        queue_reserve_capacity(q, new_capacity ? new_capacity : 1);
    }
    if (q->size)
    {
        q->t = (q->t + 1) % q->capacity;
    }
    q->data[q->t] = element;
    q->size++;
}

generic_t queue_deq(queue_t *q)
{
    ASSERT_INPUT(q);
    ASSERT_MSG(q->size, "dequeuing from an empty queue");

    generic_t e = q->data[q->h];
    if (q->h != q->t)
    {
        q->h = (q->h + 1) % q->capacity;
    }
    q->size--;

    return e;
}

generic_t queue_peek(const queue_t *q)
{
    ASSERT_INPUT(q);
    ASSERT_MSG(q->size, "peeking from an empty queue");
    return q->data[q->h];
}

size_t queue_get_size(const queue_t *q)
{
    ASSERT_INPUT(q);
    return q->size;
}

size_t queue_get_capacity(const queue_t *q)
{
    ASSERT_INPUT(q);
    return q->capacity;
}

bool queue_is_empty(const queue_t *q)
{
    ASSERT_INPUT(q);
    return q->size == 0;
}

void queue_reserve_capacity(queue_t *q, size_t new_capacity)
{
    ASSERT_INPUT(q);

    /* If queue is not empty and we need more slots we allocate
     * additional memory and move existing elements to bigger room.
     * Old queue memory should be freed.
     */
    if (q->size)
    {
        generic_t *p = umalloc(new_capacity * sizeof(*p));
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

void queue_serialize(const queue_t *q, buffer_t *buf)
{
    ASSERT_INPUT(q);
    ASSERT_INPUT(buf);

    buffer_append_byte(buf, '[');
    for (size_t i = 0; i < q->size; i++)
    {
        generic_serialize(q->data[(q->h + i) % q->capacity], buf);
        if (i < q->size - 1)
        {
            buffer_append_data(buf, ", ", 2);
        }
    }
    buffer_append_byte(buf, ']');
}

char *queue_as_str(const queue_t *q)
{
    ASSERT_INPUT(q);

    buffer_t buf = {0};
    queue_serialize(q, &buf);
    buffer_null_terminate(&buf);

    return buf.data;
}

int queue_fprint(const queue_t *q, FILE *out)
{
    ASSERT_INPUT(q);
    ASSERT_INPUT(out);

    char *str = queue_as_str(q);
    int ret = fprintf(out, "%s\n", str);
    ufree(str);

    return ret;
}

int queue_print(const queue_t *q)
{
    ASSERT_INPUT(q);
    return queue_fprint(q, stdout);
}
