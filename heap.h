#ifndef HEAP_H__
#define HEAP_H__

#include "generic.h"

typedef struct heap_opaq heap_t;

// For custom void * data you can use either one by
// providing appropriate comparator but for generic_t
// native types the default comparator always corresponds
// to MIN_HEAP. So there is function which allows to
// explicitely set heap type.
typedef enum {
    MAX_HEAP = -1,
    MIN_HEAP = 1, // default
} heap_type_t;

heap_t *heap_create(void);
heap_t *heap_create_ext(size_t capacity, heap_type_t);
void register_comparator(heap_t *h, void_cmp_t *cmp);
void heap_destroy(heap_t *h);
void heap_clear(heap_t *h);
void heap_push(heap_t *h, generic_t e);
generic_t heap_pop(heap_t *h);
size_t heap_get_size(const heap_t *h);
bool heap_is_empty(const heap_t *h);
generic_t heap_peek(const heap_t *h);

size_t heap_get_capacity(const heap_t *h);
void heap_reserve_capacity(heap_t *h, size_t new_capacity);

void heap_take_data_ownership(heap_t *h);
void heap_drop_data_ownership(heap_t *h);
void heap_set_destroyer(heap_t *h, void_dtr_t dtr);
void heap_set_comparator(heap_t *h, void_cmp_t cmp);
void heap_set_copier(heap_t *h, void_cpy_t cpy);
void_dtr_t heap_get_destroyer(const heap_t *h);
void_cmp_t heap_get_comparator(const heap_t *h);
void_cpy_t heap_get_copier(const heap_t *h);

void heap_dump_to_dot(const heap_t *h, const char *name, FILE *out);

#endif
