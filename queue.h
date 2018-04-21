#ifndef UQUEUE_H__
#define UQUEUE_H__

#include "generic.h"

#define QUEUE_INITIAL_CAPACITY 16

typedef struct uqueue_opaq uqueue_t;

uqueue_t *uqueue_create(void);
void uqueue_destroy(uqueue_t *q);
void uqueue_reserve_capacity(uqueue_t *q, size_t new_capacity);
void uqueue_clear(uqueue_t *q);
void uqueue_enq(uqueue_t *q, ugeneric_t element);
ugeneric_t uqueue_peek(const uqueue_t *q);
ugeneric_t uqueue_deq(uqueue_t *q);
size_t uqueue_get_size(const uqueue_t *q);
size_t uqueue_get_capacity(const uqueue_t *q);
bool uqueue_is_empty(const uqueue_t *q);

char *uqueue_as_str(const uqueue_t *q);
void uqueue_serialize(const uqueue_t *q, ubuffer_t *buf);
int uqueue_fprint(const uqueue_t *q, FILE *out);
int uqueue_print(const uqueue_t *q);

static void uqueue_take_data_ownership(uqueue_t *q);
static void uqueue_drop_data_ownership(uqueue_t *q);
static bool uqueue_is_data_owner(uqueue_t *q);

ugeneric_base_t *uqueue_get_base(uqueue_t *q);
DEFINE_BASE_FUNCS(uqueue)

#endif
