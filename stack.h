#ifndef STACK_H__
#define STACK_H__

#include "generic.h"

typedef struct stack_opaq stack_t;

stack_t *stack_create(void);
void stack_destroy(stack_t *s);
generic_t stack_pop(stack_t *s);
generic_t stack_peek(const stack_t *s);
void stack_push(stack_t *s, generic_t e);
size_t stack_get_size(const stack_t *s);
bool stack_is_empty(const stack_t *s);
void stack_reserve_capacity(stack_t *s, size_t capacity);
size_t stack_get_capacity(const stack_t *s);

#endif
