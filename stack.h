#ifndef USTACK_H__
#define USTACK_H__

#include "generic.h"

typedef struct ustack_opaq ustack_t;

ustack_t *ustack_create(void);
void ustack_destroy(ustack_t *s);
ugeneric_t ustack_pop(ustack_t *s);
ugeneric_t ustack_peek(const ustack_t *s);
void ustack_push(ustack_t *s, ugeneric_t e);
size_t ustack_get_size(const ustack_t *s);
bool ustack_is_empty(const ustack_t *s);
void ustack_reserve_capacity(ustack_t *s, size_t capacity);
size_t ustack_get_capacity(const ustack_t *s);

static void ustack_take_data_ownership(ustack_t *s);
static void ustack_drop_data_ownership(ustack_t *s);
static bool ustack_is_data_owner(ustack_t *s);

ugeneric_base_t *ustack_get_base(ustack_t *s);
DEFINE_BASE_FUNCS(ustack)

#endif
