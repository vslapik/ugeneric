#ifndef DSU_H__
#define DSU_H__

#include "generic.h"

typedef struct dsu_opaq dsu_t;

dsu_t *dsu_create(size_t size);
bool dsu_is_united(dsu_t *d, size_t p, size_t);
void dsu_unite(dsu_t *d, size_t p, size_t q);
void dsu_destroy(dsu_t *d);

#endif
