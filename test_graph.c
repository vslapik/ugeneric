#include "graph.h"

#include "file_utils.h"
#include "mem.h"
#include "string_utils.h"
#include "ut_utils.h"
#include "vector.h"

void test_graph(bool verbose)
{
    ugraph_t *g = ugraph_create(100, UGRAPH_UNDIRECTED);
    ugraph_add_edge(g, 0, 98);
    ugraph_add_edge(g, 1, 98);
    ugraph_add_edge(g, 6, 34);
    ugraph_add_edge(g, 6, 35);
    ugraph_add_edge(g, 6, 36);
    ugraph_add_edge(g, 6, 39);
    if (verbose)
        ugraph_dump_to_dot(g, "test", stdout);
    ugraph_destroy(g);
}

void test_mincut(bool verbose)
{
    // small graph -----------------------------------------------------------
    ugraph_t *g = ugraph_create(7, UGRAPH_UNDIRECTED);
    ugraph_add_edge(g, 0, 1);
    ugraph_add_edge(g, 0, 3);
    ugraph_add_edge(g, 2, 3);
    ugraph_add_edge(g, 2, 1);
    ugraph_add_edge(g, 4, 1);
    ugraph_add_edge(g, 4, 2);
    ugraph_add_edge(g, 4, 3);
    ugraph_add_edge(g, 5, 1);
    ugraph_add_edge(g, 5, 2);
    ugraph_add_edge(g, 5, 3);
    ugraph_add_edge(g, 5, 4);
    ugraph_add_edge(g, 6, 1);
    ugraph_add_edge(g, 6, 2);
    ugraph_add_edge(g, 6, 3);
    ugraph_add_edge(g, 6, 4);
    ugraph_add_edge(g, 6, 5);

    uvector_t *mincut = ugraph_get_min_cut(g, 100);
    size_t cutsize = uvector_get_size(mincut);
    if (verbose)
    {
        printf("%zd\n", cutsize);
        for (size_t i = 0; i < cutsize; i++)
        {
            ugraph_edge_t *e = G_AS_PTR(uvector_get_at(mincut, i));
            printf("%zu -- %zu\n", e->f, e->t);
        }
    }
    UASSERT(cutsize == 2);
    ugraph_destroy(g);
    uvector_destroy(mincut);

    // large graph ------------------------------------------------------------
    const char *path = "utdata/kargerMinCut.txt";
    ugeneric_t t = ufile_read_lines(path);
    UASSERT_NO_ERROR(t);
    uvector_t *v = G_AS_PTR(t);
    size_t vlen = uvector_get_size(v);
    g = ugraph_create(vlen, UGRAPH_UNDIRECTED);

    for (size_t i = 0; i < vlen; i++)
    {
        char *row = G_AS_PTR(uvector_get_at(v, i));
        uvector_t *va = ustring_split(row, "\t");
        size_t valen = uvector_get_size(va) - 1; // -1 to ignore trailing \t in utdata
        for (size_t j = 1; j < valen; j++)
        {
            char *nn = G_AS_PTR(uvector_get_at(va, j));
            ugraph_add_edge(g, i, atoi(nn) - 1);
        }
        uvector_destroy(va);
    }
    uvector_destroy(v);

    mincut = ugraph_get_min_cut(g, 200);
    cutsize = uvector_get_size(mincut);
    UASSERT_INT_EQ(cutsize, 17);

    if (verbose)
    {
        printf("%zd\n", cutsize);
        ugraph_dump_to_dot(g, "karger", stdout);
    }

    ugraph_destroy(g);
    uvector_destroy(mincut);
}

int main(int argc, char **argv)
{
    (void)argv;
    test_graph(argc > 1);
    test_mincut(argc > 1);

    return 0;
}
