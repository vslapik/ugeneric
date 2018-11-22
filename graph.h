#ifndef UGRAPH_H__
#define UGRAPH_H__

#include "dsu.h"
#include "generic.h"
#include "vector.h"

typedef struct ugraph_opaq ugraph_t;
typedef struct ugraph_edge_iterator_opaq ugraph_edge_iterator_t;

typedef enum {
    UGRAPH_DIRECTED,
    UGRAPH_UNDIRECTED,
} ugraph_type_t;

typedef struct {
    size_t f; // from
    size_t t; // to
    int w;    // weight
} ugraph_edge_t;

typedef bool (*ugraph_node_callback_t)(const ugraph_t *g, size_t n, void *data);

ugraph_t *ugraph_create(size_t n, ugraph_type_t type);
void ugraph_add_edge(ugraph_t *g, size_t from, size_t to, int weight);
//void ugraph_add_vertex(ugraph_t *g); // ?
void ugraph_remove_edge(ugraph_t *g, size_t from, size_t to);
//void ugraph_remove_vertex(ugraph_t *g, size_t f, size_t); // ?
const ugraph_edge_t *ugraph_get_edge(const ugraph_t *g, size_t from, size_t to);
size_t ugraph_get_edge_count(const ugraph_t *g);
size_t ugraph_get_vertex_count(const ugraph_t *g);
uvector_t *ugraph_get_vertices(const ugraph_t *g);
uvector_t *ugraph_get_edges(const ugraph_t *g); // vector of *ugraph_edge_t
uvector_t *ugraph_get_min_cut(const ugraph_t *g, size_t iterations);
uvector_t *ugraph_get_topological_order(const ugraph_t *g);
uvector_t *ugraph_get_strongly_connected_components(const ugraph_t *g);

typedef void (*ugraph_search)(const ugraph_t *g, size_t root, ugraph_node_callback_t cb, void *data);
void ugraph_bfs(const ugraph_t *g, size_t root, ugraph_node_callback_t cb, void *data);
void ugraph_dfs_preorder(const ugraph_t *g, size_t root, ugraph_node_callback_t cb, void *data);
void ugraph_dfs_postorder(const ugraph_t *g, size_t root, ugraph_node_callback_t cb, void *data);

uvector_t *ugraph_dijkstra(const ugraph_t *g, size_t from, size_t to);
int ugraph_compute_path_length(const ugraph_t *g, const uvector_t *path);

ugraph_t *ugraph_get_prims_mst(const ugraph_t *g);
ugraph_t *ugraph_get_kruskal_mst(const ugraph_t *g);
udsu_t *ugraph_get_max_space_clustering(const ugraph_t *g, size_t k);

void ugraph_dump_to_dot(const ugraph_t *g, const char *name, FILE *out);
//ugraph_t ugraph_load_from_dot(FILE *in);

void ugraph_destroy(ugraph_t *g);

ugraph_edge_iterator_t *ugraph_edge_iterator_create(const ugraph_t *g, size_t n);
const ugraph_edge_t *ugraph_edge_iterator_get_next(ugraph_edge_iterator_t *ei);
bool ugraph_edge_iterator_has_next(const ugraph_edge_iterator_t *ei);
void ugraph_edge_iterator_reset(ugraph_edge_iterator_t *ei);
void ugraph_edge_iterator_destroy(ugraph_edge_iterator_t *ei);

#endif
