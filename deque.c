#include <stdlib.h>
#include <string.h>
#include "deque.h"

struct deque_opaq {
    void *map;
    size_t map_size;
    size_t size;
    bool is_data_owner;
    void_cpy_t cpy;
    void_cmp_t cmp;
    void_dtr_t dtr;
};

deque_t *deque_create_empty(void)
{
    deque_t *q = NULL;
    return q;
}

deque_t *deque_create(size_t size, ugeneric_t value)
{
    (void)size;
    (void)value;
    deque_t *q = NULL;
    return q;
}

