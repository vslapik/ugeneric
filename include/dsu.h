#ifndef UDSU_H__
#define UDSU_H__

#include "generic.h"

typedef struct udsu_opaq udsu_t;

udsu_t *udsu_create(size_t size);
bool udsu_is_united(const udsu_t *d, size_t p, size_t q);
void udsu_unite(udsu_t *d, size_t p, size_t q);
size_t udsu_get_rank(const udsu_t *d);
void udsu_destroy(udsu_t *d);

#endif
