#include "graph.h"

#include "file_utils.h"
#include "mem.h"
#include "string_utils.h"
#include "ut_utils.h"
#include "vector.h"

void test_graph(bool verbose)
{
    ugraph_t *g = ugraph_create(100, UGRAPH_UNDIRECTED);
    ugraph_add_edge(g, 0, 98, 1);
    ugraph_add_edge(g, 1, 98, 1);
    ugraph_add_edge(g, 6, 34, 1);
    ugraph_add_edge(g, 6, 35, 1);
    ugraph_add_edge(g, 6, 36, 1);
    ugraph_add_edge(g, 6, 39, 1);
    if (verbose)
        ugraph_dump_to_dot(g, "test", stdout);
    ugraph_destroy(g);
}

void test_mincut(bool verbose)
{
    // small graph -----------------------------------------------------------
    ugraph_t *g = ugraph_create(7, UGRAPH_UNDIRECTED);
    ugraph_add_edge(g, 0, 1, 1);
    ugraph_add_edge(g, 0, 3, 1);
    ugraph_add_edge(g, 2, 3, 1);
    ugraph_add_edge(g, 2, 1, 1);
    ugraph_add_edge(g, 4, 1, 1);
    ugraph_add_edge(g, 4, 2, 1);
    ugraph_add_edge(g, 4, 3, 1);
    ugraph_add_edge(g, 5, 1, 1);
    ugraph_add_edge(g, 5, 2, 1);
    ugraph_add_edge(g, 5, 3, 1);
    ugraph_add_edge(g, 5, 4, 1);
    ugraph_add_edge(g, 6, 1, 1);
    ugraph_add_edge(g, 6, 2, 1);
    ugraph_add_edge(g, 6, 3, 1);
    ugraph_add_edge(g, 6, 4, 1);
    ugraph_add_edge(g, 6, 5, 1);

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
            ugraph_add_edge(g, i, atoi(nn) - 1, 1);
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

static bool __remember_node(const ugraph_t *g, size_t n, void *data)
{
    (void)g;
    uvector_t *nodes = data;
    uvector_append(nodes, G_INT(n));
    return false;
}

static uvector_t *__get_path(const ugraph_t *g, size_t root, ugraph_search f)
{
    uvector_t *nodes = uvector_create();
    f(g, root, __remember_node, nodes);
    return nodes;
}

static void __check_search(const ugraph_t *g, size_t root, const char *exp, ugraph_search f)
{
    uvector_t *path = __get_path(g, root, f);
    uvector_sort(path);
    char *str = uvector_as_str(path);
    UASSERT_STR_EQ(str, exp);
    ufree(str);
    uvector_destroy(path);
}

static void _check_search(ugraph_t *g, size_t root, const char *exp)
{
    __check_search(g, root, exp, ugraph_bfs);
    __check_search(g, root, exp, ugraph_dfs_preorder);
    __check_search(g, root, exp, ugraph_dfs_postorder);
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
    ugraph_add_edge(g, 0, 1, 1);
    _check_search(g, 0, "[0, 1]");
    _check_search(g, 1, "[1]");
    ugraph_destroy(g);

    g = ugraph_create(4, UGRAPH_DIRECTED);
    ugraph_add_edge(g, 0, 1, 1);
    ugraph_add_edge(g, 0, 2, 1);
    ugraph_add_edge(g, 0, 3, 1);
    _check_search(g, 0, "[0, 1, 2, 3]");
    _check_search(g, 1, "[1]");
    _check_search(g, 2, "[2]");
    _check_search(g, 3, "[3]");
    ugraph_destroy(g);

    g = ugraph_create(4, UGRAPH_DIRECTED);
    ugraph_add_edge(g, 0, 1, 1);
    ugraph_add_edge(g, 1, 2, 1);
    ugraph_add_edge(g, 2, 3, 1);
    _check_search(g, 0, "[0, 1, 2, 3]");
    _check_search(g, 1, "[1, 2, 3]");
    _check_search(g, 2, "[2, 3]");
    _check_search(g, 3, "[3]");
    ugraph_destroy(g);

    g = ugraph_create(7, UGRAPH_UNDIRECTED);
    ugraph_add_edge(g, 0, 1, 1);
    ugraph_add_edge(g, 0, 3, 1);
    ugraph_add_edge(g, 2, 3, 1);
    ugraph_add_edge(g, 2, 1, 1);
    ugraph_add_edge(g, 4, 1, 1);
    ugraph_add_edge(g, 4, 2, 1);
    ugraph_add_edge(g, 4, 3, 1);
    ugraph_add_edge(g, 5, 4, 1);
    ugraph_add_edge(g, 6, 3, 1);
    ugraph_add_edge(g, 6, 5, 1);
    _check_search(g, 0, "[0, 1, 2, 3, 4, 5, 6]");
    _check_search(g, 1, "[0, 1, 2, 3, 4, 5, 6]");
    _check_search(g, 2, "[0, 1, 2, 3, 4, 5, 6]");
    _check_search(g, 3, "[0, 1, 2, 3, 4, 5, 6]");
    _check_search(g, 4, "[0, 1, 2, 3, 4, 5, 6]");
    _check_search(g, 5, "[0, 1, 2, 3, 4, 5, 6]");
    _check_search(g, 6, "[0, 1, 2, 3, 4, 5, 6]");
    ugraph_destroy(g);

    g = ugraph_create(7, UGRAPH_DIRECTED);
    ugraph_add_edge(g, 0, 1, 1);
    ugraph_add_edge(g, 0, 3, 1);
    ugraph_add_edge(g, 2, 3, 1);
    ugraph_add_edge(g, 2, 1, 1);
    ugraph_add_edge(g, 4, 1, 1);
    ugraph_add_edge(g, 4, 2, 1);
    ugraph_add_edge(g, 4, 3, 1);
    ugraph_add_edge(g, 5, 4, 1);
    ugraph_add_edge(g, 6, 3, 1);
    ugraph_add_edge(g, 6, 5, 1);
    _check_search(g, 0, "[0, 1, 3]");
    _check_search(g, 1, "[1]");
    _check_search(g, 2, "[1, 2, 3]");
    _check_search(g, 3, "[3]");
    _check_search(g, 4, "[1, 2, 3, 4]");
    _check_search(g, 5, "[1, 2, 3, 4, 5]");
    _check_search(g, 6, "[1, 2, 3, 4, 5, 6]");
    ugraph_destroy(g);
}

bool _print_node(const ugraph_t *g, size_t n, void *data)
{
    (void)g;
    (void)data;
    printf("%zd\n", n);

    return false;
}

static void _check_dijkstra(const ugraph_t *g, size_t from, size_t to,
                            const char *exp_path, size_t exp_len)
{
    uvector_t *p = ugraph_dijkstra(g, from, to);
    char *path = uvector_as_str(p);
    int len = ugraph_compute_path_length(g, p);
    UASSERT_INT_EQ(len, exp_len);
    UASSERT_STR_EQ(path, exp_path);
    //printf("----- node %zd, len: %d\n", to, len);
    //uvector_print(p);

    uvector_destroy(p);
    ufree(path);
}

void test_dijkstra(bool verbose)
{
    (void)verbose;
    ugraph_t *g;

    g = ugraph_create(1, UGRAPH_UNDIRECTED);
    _check_dijkstra(g, 0, 0, "[0]", 0);
    ugraph_destroy(g);

    g = ugraph_create(1, UGRAPH_DIRECTED);
    _check_dijkstra(g, 0, 0, "[0]", 0);
    ugraph_destroy(g);

    g = ugraph_create(2, UGRAPH_UNDIRECTED);
    ugraph_add_edge(g, 0, 1, 100);
    _check_dijkstra(g, 0, 1, "[0, 1]", 100);
    ugraph_destroy(g);

    g = ugraph_create(2, UGRAPH_DIRECTED);
    ugraph_add_edge(g, 0, 1, 100);
    _check_dijkstra(g, 0, 1, "[0, 1]", 100);
    _check_dijkstra(g, 1, 0, "[]", 0);
    ugraph_destroy(g);

    g = ugraph_create(7, UGRAPH_DIRECTED);
    ugraph_add_edge(g, 0, 5, 1);
    ugraph_add_edge(g, 0, 4, 1);
    ugraph_add_edge(g, 0, 3, 1);
    ugraph_add_edge(g, 0, 2, 1);
    ugraph_add_edge(g, 0, 1, 1);
    ugraph_add_edge(g, 5, 6, 5);
    ugraph_add_edge(g, 4, 6, 4);
    ugraph_add_edge(g, 3, 6, 3);
    ugraph_add_edge(g, 2, 6, 2);
    ugraph_add_edge(g, 1, 6, 1);
    _check_dijkstra(g, 0, 6, "[0, 1, 6]", 2);
    ugraph_destroy(g);

    g = ugraph_create(7, UGRAPH_UNDIRECTED);
    ugraph_add_edge(g, 0, 1, 1);
    ugraph_add_edge(g, 0, 3, 1);
    ugraph_add_edge(g, 2, 3, 1);
    ugraph_add_edge(g, 2, 1, 1);
    ugraph_add_edge(g, 4, 2, 1);
    ugraph_add_edge(g, 4, 3, 1);
    ugraph_add_edge(g, 5, 4, 1);
    ugraph_add_edge(g, 6, 3, 1);
    ugraph_add_edge(g, 6, 5, 1);
    ugraph_add_edge(g, 2, 6, 1);
    _check_dijkstra(g, 0, 6, "[0, 3, 6]", 2);
    _check_dijkstra(g, 0, 1, "[0, 1]", 1);
    ugraph_destroy(g);

}

void test_dijkstra_large(bool verbose)
{
    (void)verbose;
    ugraph_t *g;
    const char *path = "utdata/dijkstraData.txt";
    ugeneric_t t = ufile_read_lines(path, "\n");
    UASSERT_NO_ERROR(t);
    uvector_t *v = G_AS_PTR(t);
    size_t vlen = uvector_get_size(v);
    g = ugraph_create(vlen, UGRAPH_UNDIRECTED);

    for (size_t i = 0; i < vlen; i++)
    {
        char *row = G_AS_PTR(uvector_get_at(v, i));
        uvector_t *va = ustring_split(row, "\t");
        ugeneric_destroy_v(uvector_pop_back(va), NULL); // to ignore empty string parsed after last \t
        size_t valen = uvector_get_size(va);
        for (size_t j = 0; j < valen; j++)
        {
            if (j == 0)
            {
                // First number is a vertex number.
                UASSERT_G_EQ(G_INT(i + 1), ugeneric_parse(G_AS_STR(uvector_get_at(va, j))));
            }
            else
            {
                char *nn = G_AS_PTR(uvector_get_at(va, j));
                uvector_t *tw = ustring_split(nn, ",");
                UASSERT(uvector_get_size(tw) == 2);

                ugeneric_t gt = ugeneric_parse(G_AS_PTR(uvector_get_at(tw, 0)));
                ugeneric_t gw = ugeneric_parse(G_AS_PTR(uvector_get_at(tw, 1)));
                UASSERT_NO_ERROR(gt);
                UASSERT_NO_ERROR(gw);

                ugraph_add_edge(g, i, G_AS_INT(gt) - 1, G_AS_INT(gw));

                uvector_destroy(tw);
            }
        }
        uvector_destroy(va);
    }
    uvector_destroy(v);

    //ugraph_dump_to_dot(g, "test", stdout);

    _check_dijkstra(g, 0, 6, "[0, 113, 128, 84, 52, 6]", 2599);
    _check_dijkstra(g, 0, 36, "[0, 144, 107, 125, 154, 36]", 2610);
    _check_dijkstra(g, 0, 58, "[0, 91, 193, 161, 58]", 2947);
    _check_dijkstra(g, 0, 81, "[0, 91, 133, 134, 81]", 2052);
    _check_dijkstra(g, 0, 98, "[0, 98]", 2367);
    _check_dijkstra(g, 0, 114, "[0, 79, 114]", 2399);
    _check_dijkstra(g, 0, 132, "[0, 113, 128, 84, 132]", 2029);
    _check_dijkstra(g, 0, 164, "[0, 79, 18, 186, 164]", 2442);
    _check_dijkstra(g, 0, 187, "[0, 91, 69, 8, 71, 156, 25, 94, 195, 187]", 2505);
    _check_dijkstra(g, 0, 196, "[0, 113, 102, 109, 196]", 3068);

    ugraph_destroy(g);
}

// Ensures topological ordering in path for graph g.
static void _check_order(const ugraph_t *g, uvector_t *path, bool has_loop)
{
    uvector_print(path);

    if (!has_loop)
    {
        const ugraph_edge_t *e = NULL;
        size_t len = uvector_get_size(path);
        for (size_t i = 0; i + 1 < len; i++)
        {
            for (size_t j = i + 1; j < len; j++)
            {
                size_t from = G_AS_SIZE(uvector_get_at(path, i));
                size_t to = G_AS_SIZE(uvector_get_at(path, j));
                //printf("from: %zu, to:%zu\n", from, to);
                e = ugraph_get_edge(g, from, to);
                if (e)
                {
                    UASSERT_SIZE_EQ(from, e->f);
                    UASSERT_SIZE_EQ(to, e->t);
                }
                // There shouldn't be an edge which breaks
                // the  topological order.
                e = ugraph_get_edge(g, to, from);
                UASSERT(!e);
            }
        }
    }
    else
    {
        // TODO: fix cycle detection

        // Empty vector is expected for grap which has a loop.
        //UASSERT_SIZE_EQ(uvector_get_size(path), 0);
    }
}

void test_topological_ordering(bool verbose)
{
    (void)verbose;

    uvector_t *o;
    ugraph_t *g;

    g = ugraph_create(1, UGRAPH_DIRECTED);
    o = ugraph_get_topological_order(g);
    _check_order(g, o, false);
    uvector_destroy(o);
    ugraph_destroy(g);

    //
    g = ugraph_create(3, UGRAPH_DIRECTED);
    ugraph_add_edge(g, 0, 1, 1);
    ugraph_add_edge(g, 1, 2, 1);
    o = ugraph_get_topological_order(g);
    _check_order(g, o, false);
    uvector_destroy(o);
    ugraph_destroy(g);

    //
    g = ugraph_create(3, UGRAPH_DIRECTED);
    ugraph_add_edge(g, 0, 1, 1);
    ugraph_add_edge(g, 0, 2, 1);
    ugraph_add_edge(g, 1, 2, 1);
    o = ugraph_get_topological_order(g);
    _check_order(g, o, false);
    uvector_destroy(o);
    ugraph_destroy(g);

    //
    g = ugraph_create(8, UGRAPH_DIRECTED);
    ugraph_add_edge(g, 0, 3, 1);
    ugraph_add_edge(g, 3, 5, 1);
    ugraph_add_edge(g, 1, 4, 1);
    ugraph_add_edge(g, 4, 6, 1);
    ugraph_add_edge(g, 2, 7, 1);
    ugraph_add_edge(g, 2, 4, 1);
    ugraph_add_edge(g, 3, 6, 1);
    ugraph_add_edge(g, 3, 7, 1);
    o = ugraph_get_topological_order(g);
    _check_order(g, o, false);
    uvector_destroy(o);
    ugraph_destroy(g);

    // loop
    g = ugraph_create(3, UGRAPH_DIRECTED);
    ugraph_add_edge(g, 0, 1, 1);
    ugraph_add_edge(g, 1, 2, 1);
    ugraph_add_edge(g, 2, 0, 1);
    o = ugraph_get_topological_order(g);
    _check_order(g, o, true);
    uvector_destroy(o);
    ugraph_destroy(g);
}

int main(int argc, char **argv)
{
    (void)argv;

    test_graph(argc > 1);
    test_mincut(argc > 1);
    test_search(argc > 1);
    test_dijkstra(argc > 1);
    test_dijkstra_large(argc > 1);

    test_topological_ordering(argc > 1);

    return 0;
}
