#include <stdlib.h>
#include <stdbool.h>
#include "mem.h"
#include "vector.h"
#include "stack.h"

#define STACK_INITIAL_CAPACITY 16

struct ustack_opaq {
    uvector_t *data;
};

ustack_t *ustack_create(void)
{
    ustack_t *s = umalloc(sizeof(*s));
    s->data = uvector_create();
    uvector_reserve_capacity(s->data, STACK_INITIAL_CAPACITY);
    return s;
}

void ustack_take_data_ownership(ustack_t *s)
{
    uvector_take_data_ownership(s->data);
}

void ustack_drop_data_ownership(ustack_t *s)
{
    uvector_drop_data_ownership(s->data);
}

void ustack_destroy(ustack_t *s)
{
    if (s)
    {
        uvector_destroy(s->data);
        ufree(s);
    }
}

ugeneric_t ustack_pop(ustack_t *s)
{
    UASSERT_INPUT(s);
    UASSERT_INPUT(!uvector_is_empty(s->data));
    return uvector_pop_back(s->data);
}

ugeneric_t ustack_peek(const ustack_t *s)
{
    UASSERT_INPUT(s);
    UASSERT_INPUT(!uvector_is_empty(s->data));
    return uvector_get_at(s->data, uvector_get_size(s->data) - 1);
}

void ustack_push(ustack_t *s, ugeneric_t e)
{
    UASSERT_INPUT(s);
    uvector_append(s->data, e);
}

size_t ustack_get_size(const ustack_t *s)
{
    UASSERT_INPUT(s);
    return uvector_get_size(s->data);
}

bool ustack_is_empty(const ustack_t *s)
{
    UASSERT_INPUT(s);
    return uvector_is_empty(s->data);
}

void ustack_reserve_capacity(ustack_t *s, size_t capacity)
{
    UASSERT_INPUT(s);
    uvector_reserve_capacity(s->data, capacity);
}

size_t ustack_get_capacity(const ustack_t *s)
{
    UASSERT_INPUT(s);
    return uvector_get_capacity(s->data);
}

uvoid_handlers_t *ustack_get_void_handlers(ustack_t *s)
{
    UASSERT_INPUT(s);
    return uvector_get_void_handlers(s->data);
}
