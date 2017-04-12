#include <stdlib.h>
#include <string.h>
#include "generic.h"
#include "mem.h"
#include "dsu.h"
#include "vector.h"
#include "list.h"
#include "graph.h"

struct graph_opaq {
    vector_t *nodes; // adj lists
    graph_type_t type;
    size_t m; // edges
    size_t n; // nodes
};

graph_t *graph_create(size_t n, graph_type_t type)
{
    graph_t *g = umalloc(sizeof(*g));
    g->nodes = vector_create(n, G_PTR(NULL));
    vector_set_destroyer(g->nodes, (void_dtr_t)list_destroy);
    g->n = n;
    g->m = 0;
    g->type = type;

    for (size_t i = 0; i < g->n; i++)
    {
        list_t *l = list_create();
        vector_set_at(g->nodes, i, G_PTR(l));
    }

    return g;
}

static inline void swap(size_t *x, size_t *y)
{
    size_t tmp = *x;
    *x = *y;
    *y = tmp;
}

bool graph_has_edge(const graph_t *g, size_t f, size_t t)
{
    ASSERT_INPUT(g);
    ASSERT_INPUT(f < g->n);
    ASSERT_INPUT(t < g->n);

    if ((g->type == GRAPH_UNDIRECTED) && (f > t))
    {
        swap(&f, &t);
    }
    list_t *l = G_AS_PTR(vector_get_at(g->nodes, f));

    return list_contains(l, G_SIZE(t));
}

void graph_add_edge(graph_t *g, size_t f, size_t t)
{
    ASSERT_INPUT(g);
    ASSERT_INPUT(f < g->n);
    ASSERT_INPUT(t < g->n);

    if ((g->type == GRAPH_UNDIRECTED) && (f > t))
    {
        swap(&f, &t);
    }
    if (!graph_has_edge(g, f, t))
    {
        list_append(G_AS_PTR(vector_get_at(g->nodes, f)), G_SIZE(t));
        g->m += 1;
    }
}

size_t graph_get_edge_count(const graph_t *g)
{
    ASSERT_INPUT(g);
    return g->m;
}

size_t graph_get_vertex_count(const graph_t *g)
{
    ASSERT_INPUT(g);
    return g->n;
}

void graph_destroy(graph_t *g)
{
    if (g)
    {
        vector_destroy(g->nodes);
        ufree(g);
    }
}

vector_t *graph_get_edges(const graph_t *g)
{
    ASSERT_INPUT(g);

    vector_t *v = vector_create(g->m, G_NULL);
    vector_set_destroyer(v, ufree);
    size_t j = 0;
    for (size_t i = 0; i < g->n; i++)
    {
        list_t *l = G_AS_PTR(vector_get_at(g->nodes, i));
        list_iterator_t *li = list_iterator_create(l);
        while (list_iterator_has_next(li))
        {
            graph_edge_t *edge = umalloc(sizeof(*edge));
            generic_t e = list_iterator_get_next(li);
            edge->f = i;
            edge->t = G_AS_SIZE(e);
            vector_set_at(v, j++, G_PTR(edge));
        }
        list_iterator_destroy(li);
    }

    return v;
}

static vector_t *_min_cut(const graph_t *g)
{
    vector_t *edges = graph_get_edges(g); // owner of data
    vector_t *edgestmp = vector_copy(edges); // shallow copy
    size_t edges_count = graph_get_edge_count(g);
    size_t vertex_count = graph_get_vertex_count(g);
    ASSERT(vector_get_size(edges) == edges_count);

    dsu_t *d = dsu_create(vertex_count);
    while (vertex_count > 2)
    {
        size_t i = random_from_range(0, vector_get_size(edgestmp) - 1);
        graph_edge_t *ge = G_AS_PTR(vector_get_at(edgestmp, i));
        if (!dsu_is_united(d, ge->f, ge->t))
        {
            dsu_unite(d, ge->f, ge->t);
            vertex_count--;
        }
        vector_remove_at(edgestmp, i);
    }
    vector_destroy(edgestmp);

    vector_t *mincut = vector_create_empty();
    vector_set_destroyer(mincut, free);
    for (size_t i = 0; i < edges_count; i++)
    {
        graph_edge_t *e = G_AS_PTR(vector_get_at(edges, i));
        if (!dsu_is_united(d, e->f, e->t))
        {
            graph_edge_t *mincut_edge = umalloc(sizeof(*mincut_edge));
            mincut_edge->f = e->f;
            mincut_edge->t = e->t;
            vector_append(mincut, G_PTR(mincut_edge));
        }
    }
    vector_destroy(edges);
    dsu_destroy(d);

    return mincut;
}

vector_t *graph_get_min_cut(const graph_t *g, size_t iterations)
{
    vector_t *min_cut = NULL;
    vector_t *cut = NULL;
    for (size_t i = 0; i < iterations; i++)
    {
        cut = _min_cut(g);
        size_t cut_size = vector_get_size(cut);
        size_t min_cut_size = min_cut ? vector_get_size(min_cut) : SIZE_MAX;
        if (cut_size < min_cut_size)
        {
            if (min_cut)
            {
                vector_destroy(min_cut);
            }
            min_cut = cut;
        }
        else
        {
            vector_destroy(cut);
        }
    }

    return min_cut;
}

void graph_dump_to_dot(const graph_t *g, const char *name, FILE *out)
{
    ASSERT_INPUT(g);
    ASSERT_INPUT(out);

    const char *arrow = (g->type == GRAPH_UNDIRECTED) ?  "--" : "->";
    fprintf(out, "%s %s {\n",
           (g->type == GRAPH_UNDIRECTED) ? "graph" : "digraph", name);
    for (size_t i = 0; i < g->n; i++)
    {
        list_t *l = G_AS_PTR(vector_get_at(g->nodes, i));
        if (!list_is_empty(l))
        {
            list_iterator_t *li = list_iterator_create(l);
            fputs("   ", out);
            while (list_iterator_has_next(li))
            {
                generic_t g = list_iterator_get_next(li);
                fprintf(out, " %zu %s %zu;", i, arrow, G_AS_SIZE(g));
            }
            fputs("\n", out);
            list_iterator_destroy(li);
        }
    }
    fprintf(out, "}\n");
}
