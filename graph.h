#ifndef UGRAPH_H__
#define UGRAPH_H__

#include "generic.h"
#include "vector.h"

typedef struct ugraph_opaq ugraph_t;

typedef enum {
    UGRAPH_DIRECTED,
    UGRAPH_UNDIRECTED,
} ugraph_type_t;

typedef struct {
    size_t f;
    size_t t;
} ugraph_edge_t;

ugraph_t *ugraph_create(size_t n, ugraph_type_t type);
void ugraph_add_edge(ugraph_t *g, size_t from, size_t to);
//void ugraph_add_vertex(ugraph_t *g); // ?
void ugraph_remove_edge(ugraph_t *g, size_t f, size_t t);
//void ugraph_remove_vertex(ugraph_t *g, size_t f, size_t); // ?
bool ugraph_has_edge(const ugraph_t *g, size_t f, size_t t);
size_t ugraph_get_edge_count(const ugraph_t *g);
size_t ugraph_get_vertex_count(const ugraph_t *g);
uvector_t *ugraph_get_vertices(const ugraph_t *g);
uvector_t *ugraph_get_edges(const ugraph_t *g); // vector of *ugraph_type_t
uvector_t *ugraph_get_min_cut(const ugraph_t *g, size_t iterations);

void ugraph_dump_to_dot(const ugraph_t *g, const char *name, FILE *out);
ugraph_t ugraph_load_from_dot(FILE *in);

void ugraph_destroy(ugraph_t *g);

#endif
