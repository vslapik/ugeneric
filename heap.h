#ifndef HEAP_H__
#define HEAP_H__

#include "generic.h"

typedef struct heap_opaq heap_t;

heap_t *heap_create(void);
heap_t *heap_create_with_capacity(size_t capacity);
void register_comparator(heap_t *h, void_cmp_t *cmp);
void heap_destroy(heap_t *h);
void heap_clear(heap_t *h);
void heap_push(heap_t *h, generic_t e);
generic_t heap_pop(heap_t *h);
size_t heap_get_size(const heap_t *h);
bool heap_is_empty(const heap_t *h);
generic_t heap_peek(const heap_t *h);

#endif
