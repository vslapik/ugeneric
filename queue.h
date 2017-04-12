#ifndef QUEUE_H__
#define QUEUE_H__

#include "generic.h"

#define QUEUE_INITIAL_CAPACITY 16

typedef struct queue_opaq queue_t;

queue_t *queue_create(void);
void queue_destroy(queue_t *q);
void queue_reserve_capacity(queue_t *q, size_t new_capacity);
void queue_clear(queue_t *q);
void queue_enq(queue_t *q, generic_t element);
generic_t queue_peek(const queue_t *q);
generic_t queue_deq(queue_t *q);
size_t queue_get_size(const queue_t *q);
size_t queue_get_capacity(const queue_t *q);
bool queue_is_empty(const queue_t *q);

char *queue_as_str(const queue_t *q);
void queue_serialize(const queue_t *q, buffer_t *buf);
int queue_fprint(const queue_t *q, FILE *out);
int queue_print(const queue_t *q);

#endif
