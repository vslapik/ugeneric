#ifndef UHEAP_H__
#define UHEAP_H__

#include "generic.h"
#include "void.h"

typedef struct uheap_opaq uheap_t;

// For custom void * data you can use either one by
// providing appropriate comparator but for ugeneric_t
// native types the default comparator always corresponds
// to MIN_HEAP. So there is function which allows to
// explicitely set heap type.
typedef enum {
    UHEAP_TYPE_MAX = -1,
    UHEAP_TYPE_MIN = 1, // default
} uheap_type_t;

uheap_t *uheap_create(void);
uheap_t *uheap_create_ext(size_t capacity, uheap_type_t);
void uheap_destroy(uheap_t *h);
void uheap_clear(uheap_t *h);
void uheap_push(uheap_t *h, ugeneric_t e);
ugeneric_t uheap_pop(uheap_t *h);
size_t uheap_get_size(const uheap_t *h);
bool uheap_is_empty(const uheap_t *h);
ugeneric_t uheap_peek(const uheap_t *h);

size_t uheap_get_capacity(const uheap_t *h);
void uheap_reserve_capacity(uheap_t *h, size_t new_capacity);
ugeneric_t *uheap_get_cells(const uheap_t *h);

void uheap_take_data_ownership(uheap_t *h);
void uheap_drop_data_ownership(uheap_t *h);

void uheap_dump_to_dot(const uheap_t *h, const char *name, FILE *out);

uvoid_handlers_t *uheap_get_void_handlers(uheap_t *h);
DECLARE_VOID_FUNCS(uheap)

#endif
