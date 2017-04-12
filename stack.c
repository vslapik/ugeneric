#include <stdlib.h>
#include <stdbool.h>
#include "mem.h"
#include "vector.h"
#include "stack.h"

#define STACK_INITIAL_CAPACITY 16

struct stack_opaq {
    vector_t *data;
};

stack_t *stack_create()
{
    stack_t *s = umalloc(sizeof(*s));
    s->data = vector_create_empty();
    vector_reserve_capacity(s->data, STACK_INITIAL_CAPACITY);

    return s;
}

void stack_destroy(stack_t *s)
{
    if (s)
    {
        vector_destroy(s->data);
        ufree(s);
    }
}

generic_t stack_pop(stack_t *s)
{
    ASSERT_INPUT(s);
    ASSERT_INPUT(!vector_is_empty(s->data));
    return vector_pop_back(s->data);
}

generic_t stack_peek(const stack_t *s)
{
    ASSERT_INPUT(s);
    ASSERT_INPUT(!vector_is_empty(s->data));
    return vector_get_at(s->data, vector_get_size(s->data) - 1);
}

void stack_push(stack_t *s, generic_t e)
{
    ASSERT_INPUT(s);
    vector_append(s->data, e);
}

size_t stack_get_size(const stack_t *s)
{
    ASSERT_INPUT(s);
    return vector_get_size(s->data);
}

bool stack_is_empty(const stack_t *s)
{
    ASSERT_INPUT(s);
    return vector_is_empty(s->data);
}

void stack_reserve_capacity(stack_t *s, size_t capacity)
{
    ASSERT_INPUT(s);
    vector_reserve_capacity(s->data, capacity);
}

size_t stack_get_capacity(const stack_t *s)
{
    ASSERT_INPUT(s);
    return vector_get_capacity(s->data);
}
