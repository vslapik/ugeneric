#ifndef GRAPH_H__
#define GRAPH_H__

#include "generic.h"
#include "vector.h"

typedef struct graph_opaq graph_t;

typedef enum {
    GRAPH_DIRECTED,
    GRAPH_UNDIRECTED,
} graph_type_t;

typedef struct {
    size_t f;
    size_t t;
} graph_edge_t;

graph_t *graph_create(size_t n, graph_type_t type);
void graph_add_edge(graph_t *g, size_t from, size_t to);
//void graph_add_vertex(graph_t *g); // ?
void graph_remove_edge(graph_t *g, size_t f, size_t t);
//void graph_remove_vertex(graph_t *g, size_t f, size_t); // ?
bool graph_has_edge(const graph_t *g, size_t f, size_t t);
size_t graph_get_edge_count(const graph_t *g);
size_t graph_get_vertex_count(const graph_t *g);
vector_t *graph_get_vertices(const graph_t *g);
vector_t *graph_get_edges(const graph_t *g); // vector of *graph_type_t
vector_t *graph_get_min_cut(const graph_t *g, size_t iterations);

void graph_dump_to_dot(const graph_t *g, const char *name, FILE *out);
graph_t graph_load_from_dot(FILE *in);

void graph_destroy(graph_t *g);

#endif
