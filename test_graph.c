#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "mem.h"
#include "graph.h"
#include "vector.h"
#include "string_utils.h"
#include "file_utils.h"
#include "ut_utils.h"

void test_graph(bool verbose)
{
    graph_t *g = graph_create(100, GRAPH_UNDIRECTED);
    graph_add_edge(g, 0, 98);
    graph_add_edge(g, 1, 98);
    graph_add_edge(g, 6, 34);
    graph_add_edge(g, 6, 35);
    graph_add_edge(g, 6, 36);
    graph_add_edge(g, 6, 39);
    if (verbose)
        graph_dump_to_dot(g, "test", stdout);
    graph_destroy(g);
}

void test_mincut(bool verbose)
{
    // small graph -----------------------------------------------------------
    graph_t *g = graph_create(7, GRAPH_UNDIRECTED);
    graph_add_edge(g, 0, 1);
    graph_add_edge(g, 0, 3);
    graph_add_edge(g, 2, 3);
    graph_add_edge(g, 2, 1);
    graph_add_edge(g, 4, 1);
    graph_add_edge(g, 4, 2);
    graph_add_edge(g, 4, 3);
    graph_add_edge(g, 5, 1);
    graph_add_edge(g, 5, 2);
    graph_add_edge(g, 5, 3);
    graph_add_edge(g, 5, 4);
    graph_add_edge(g, 6, 1);
    graph_add_edge(g, 6, 2);
    graph_add_edge(g, 6, 3);
    graph_add_edge(g, 6, 4);
    graph_add_edge(g, 6, 5);

    vector_t *mincut = graph_get_min_cut(g, 100);
    size_t cutsize = vector_get_size(mincut);
    if (verbose)
    {
        printf("%zd\n", cutsize);
        for (size_t i = 0; i < cutsize; i++)
        {
            graph_edge_t *e = G_AS_PTR(vector_get_at(mincut, i));
            printf("%zu -- %zu\n", e->f, e->t);
        }
    }
    ASSERT(cutsize == 2);
    graph_destroy(g);
    vector_destroy(mincut);

    // large graph ------------------------------------------------------------
    const char *path = "utdata/kargerMinCut.txt";
    generic_t t = file_read_lines(path);
    ASSERT_NO_ERROR(t);
    vector_t *v = G_AS_PTR(t);
    size_t vlen = vector_get_size(v);
    g = graph_create(vlen, GRAPH_UNDIRECTED);

    for (size_t i = 0; i < vlen; i++)
    {
        char *row = G_AS_PTR(vector_get_at(v, i));
        vector_t *va = string_split(row, "\t");
        size_t valen = vector_get_size(va) - 1; // -1 to ignore trailing \t in utdata
        for (size_t j = 1; j < valen; j++)
        {
            char *nn = G_AS_PTR(vector_get_at(va, j));
            graph_add_edge(g, i, atoi(nn) - 1);
        }
        vector_destroy(va);
    }
    vector_destroy(v);

    mincut = graph_get_min_cut(g, 200);
    cutsize = vector_get_size(mincut);
    ASSERT_INT_EQ(cutsize, 17);

    if (verbose)
    {
        printf("%zd\n", cutsize);
        graph_dump_to_dot(g, "karger", stdout);
    }

    graph_destroy(g);
    vector_destroy(mincut);
}

int main(int argc, char **argv)
{
    (void)argv;
    test_graph(argc > 1);
    test_mincut(argc > 1);

    return 0;
}
