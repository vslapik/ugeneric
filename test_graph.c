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
    ugeneric_t t = ufile_read_lines(path, "\n");
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

bool _process_node(const ugraph_t *g, size_t n, void *data)
{
    (void)g;
    uvector_t *nodes = data;
    uvector_append(nodes, G_INT(n));
    //printf("%zd\n", n);

    return false;
}

static void __check_search(ugraph_t *g, size_t root, const char *exp, ugraph_search f)
{
    uvector_t *nodes = uvector_create();
    f(g, root, _process_node, nodes);
    uvector_sort(nodes);
    char *str = uvector_as_str(nodes);
    UASSERT_STR_EQ(str, exp);
    ufree(str);
    uvector_destroy(nodes);
}

static void _check_search(ugraph_t *g, size_t root, const char *exp)
{
    __check_search(g, root, exp, ugraph_bfs);
    __check_search(g, root, exp, ugraph_dfs);
}

void test_search(bool verbose)
{
    (void)verbose;

    ugraph_t *g;

    g = ugraph_create(1, UGRAPH_UNDIRECTED);
    _check_search(g, 0, "[0]");
    ugraph_destroy(g);

    g = ugraph_create(2, UGRAPH_DIRECTED);
    _check_search(g, 0, "[0]");
    _check_search(g, 1, "[1]");
    ugraph_add_edge(g, 0, 1);
    _check_search(g, 0, "[0, 1]");
    _check_search(g, 1, "[1]");
    ugraph_destroy(g);

    g = ugraph_create(4, UGRAPH_DIRECTED);
    ugraph_add_edge(g, 0, 1);
    ugraph_add_edge(g, 0, 2);
    ugraph_add_edge(g, 0, 3);
    _check_search(g, 0, "[0, 1, 2, 3]");
    _check_search(g, 1, "[1]");
    _check_search(g, 2, "[2]");
    _check_search(g, 3, "[3]");
    ugraph_destroy(g);

    g = ugraph_create(4, UGRAPH_DIRECTED);
    ugraph_add_edge(g, 0, 1);
    ugraph_add_edge(g, 1, 2);
    ugraph_add_edge(g, 2, 3);
    _check_search(g, 0, "[0, 1, 2, 3]");
    _check_search(g, 1, "[1, 2, 3]");
    _check_search(g, 2, "[2, 3]");
    _check_search(g, 3, "[3]");
    ugraph_destroy(g);

    g = ugraph_create(7, UGRAPH_UNDIRECTED);
    ugraph_add_edge(g, 0, 1);
    ugraph_add_edge(g, 0, 3);
    ugraph_add_edge(g, 2, 3);
    ugraph_add_edge(g, 2, 1);
    ugraph_add_edge(g, 4, 1);
    ugraph_add_edge(g, 4, 2);
    ugraph_add_edge(g, 4, 3);
    ugraph_add_edge(g, 5, 4);
    ugraph_add_edge(g, 6, 3);
    ugraph_add_edge(g, 6, 5);
    _check_search(g, 0, "[0, 1, 2, 3, 4, 5, 6]");
    _check_search(g, 1, "[0, 1, 2, 3, 4, 5, 6]");
    _check_search(g, 2, "[0, 1, 2, 3, 4, 5, 6]");
    _check_search(g, 3, "[0, 1, 2, 3, 4, 5, 6]");
    _check_search(g, 4, "[0, 1, 2, 3, 4, 5, 6]");
    _check_search(g, 5, "[0, 1, 2, 3, 4, 5, 6]");
    _check_search(g, 6, "[0, 1, 2, 3, 4, 5, 6]");
    ugraph_destroy(g);

    g = ugraph_create(7, UGRAPH_DIRECTED);
    ugraph_add_edge(g, 0, 1);
    ugraph_add_edge(g, 0, 3);
    ugraph_add_edge(g, 2, 3);
    ugraph_add_edge(g, 2, 1);
    ugraph_add_edge(g, 4, 1);
    ugraph_add_edge(g, 4, 2);
    ugraph_add_edge(g, 4, 3);
    ugraph_add_edge(g, 5, 4);
    ugraph_add_edge(g, 6, 3);
    ugraph_add_edge(g, 6, 5);
    _check_search(g, 0, "[0, 1, 3]");
    _check_search(g, 1, "[1]");
    _check_search(g, 2, "[1, 2, 3]");
    _check_search(g, 3, "[3]");
    _check_search(g, 4, "[1, 2, 3, 4]");
    _check_search(g, 5, "[1, 2, 3, 4, 5]");
    _check_search(g, 6, "[1, 2, 3, 4, 5, 6]");
    //ugraph_dump_to_dot(g, "test", stdout);
    ugraph_destroy(g);
}

int main(int argc, char **argv)
{
    (void)argv;
    test_graph(argc > 1);
    test_mincut(argc > 1);

    test_search(argc > 1);

    return 0;
}
