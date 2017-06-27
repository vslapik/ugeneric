#include <stdlib.h>
#include <string.h>
#include "generic.h"
#include "mem.h"
#include "dsu.h"
#include "vector.h"
#include "list.h"
#include "graph.h"

struct ugraph_opaq {
    uvector_t *nodes; // adj lists
    ugraph_type_t type;
    size_t m; // edges
    size_t n; // nodes
};

ugraph_t *ugraph_create(size_t n, ugraph_type_t type)
{
    ugraph_t *g = umalloc(sizeof(*g));
    g->nodes = uvector_create_with_size(n, G_PTR(NULL));
    uvector_set_destroyer(g->nodes, (void_dtr_t)ulist_destroy);
    g->n = n;
    g->m = 0;
    g->type = type;

    for (size_t i = 0; i < g->n; i++)
    {
        ulist_t *l = ulist_create();
        uvector_set_at(g->nodes, i, G_PTR(l));
    }

    return g;
}

static inline void swap(size_t *x, size_t *y)
{
    size_t tmp = *x;
    *x = *y;
    *y = tmp;
}

bool ugraph_has_edge(const ugraph_t *g, size_t f, size_t t)
{
    UASSERT_INPUT(g);
    UASSERT_INPUT(f < g->n);
    UASSERT_INPUT(t < g->n);

    if ((g->type == UGRAPH_UNDIRECTED) && (f > t))
    {
        swap(&f, &t);
    }
    ulist_t *l = G_AS_PTR(uvector_get_at(g->nodes, f));

    return ulist_contains(l, G_SIZE(t));
}

void ugraph_add_edge(ugraph_t *g, size_t f, size_t t)
{
    UASSERT_INPUT(g);
    UASSERT_INPUT(f < g->n);
    UASSERT_INPUT(t < g->n);

    if ((g->type == UGRAPH_UNDIRECTED) && (f > t))
    {
        swap(&f, &t);
    }
    if (!ugraph_has_edge(g, f, t))
    {
        ulist_append(G_AS_PTR(uvector_get_at(g->nodes, f)), G_SIZE(t));
        g->m += 1;
    }
}

size_t ugraph_get_edge_count(const ugraph_t *g)
{
    UASSERT_INPUT(g);
    return g->m;
}

size_t ugraph_get_vertex_count(const ugraph_t *g)
{
    UASSERT_INPUT(g);
    return g->n;
}

void ugraph_destroy(ugraph_t *g)
{
    if (g)
    {
        uvector_destroy(g->nodes);
        ufree(g);
    }
}

uvector_t *ugraph_get_edges(const ugraph_t *g)
{
    UASSERT_INPUT(g);

    uvector_t *v = uvector_create_with_size(g->m, G_NULL);
    uvector_set_destroyer(v, ufree);
    size_t j = 0;
    for (size_t i = 0; i < g->n; i++)
    {
        ulist_t *l = G_AS_PTR(uvector_get_at(g->nodes, i));
        ulist_iterator_t *li = ulist_iterator_create(l);
        while (ulist_iterator_has_next(li))
        {
            ugraph_edge_t *edge = umalloc(sizeof(*edge));
            ugeneric_t e = ulist_iterator_get_next(li);
            edge->f = i;
            edge->t = G_AS_SIZE(e);
            uvector_set_at(v, j++, G_PTR(edge));
        }
        ulist_iterator_destroy(li);
    }

    return v;
}

static uvector_t *_min_cut(const ugraph_t *g)
{
    uvector_t *edges = ugraph_get_edges(g); // owner of data
    uvector_t *edgestmp = uvector_copy(edges); // shallow copy
    size_t edges_count = ugraph_get_edge_count(g);
    size_t vertex_count = ugraph_get_vertex_count(g);
    UASSERT(uvector_get_size(edges) == edges_count);

    udsu_t *d = udsu_create(vertex_count);
    while (vertex_count > 2)
    {
        size_t i = ugeneric_random_from_range(0, uvector_get_size(edgestmp) - 1);
        ugraph_edge_t *ge = G_AS_PTR(uvector_get_at(edgestmp, i));
        if (!udsu_is_united(d, ge->f, ge->t))
        {
            udsu_unite(d, ge->f, ge->t);
            vertex_count--;
        }
        uvector_remove_at(edgestmp, i);
    }
    uvector_destroy(edgestmp);

    uvector_t *mincut = uvector_create();
    uvector_set_destroyer(mincut, free);
    for (size_t i = 0; i < edges_count; i++)
    {
        ugraph_edge_t *e = G_AS_PTR(uvector_get_at(edges, i));
        if (!udsu_is_united(d, e->f, e->t))
        {
            ugraph_edge_t *mincut_edge = umalloc(sizeof(*mincut_edge));
            mincut_edge->f = e->f;
            mincut_edge->t = e->t;
            uvector_append(mincut, G_PTR(mincut_edge));
        }
    }
    uvector_destroy(edges);
    udsu_destroy(d);

    return mincut;
}

uvector_t *ugraph_get_min_cut(const ugraph_t *g, size_t iterations)
{
    uvector_t *min_cut = NULL;
    uvector_t *cut = NULL;
    for (size_t i = 0; i < iterations; i++)
    {
        cut = _min_cut(g);
        size_t cut_size = uvector_get_size(cut);
        size_t min_cut_size = min_cut ? uvector_get_size(min_cut) : SIZE_MAX;
        if (cut_size < min_cut_size)
        {
            if (min_cut)
            {
                uvector_destroy(min_cut);
            }
            min_cut = cut;
        }
        else
        {
            uvector_destroy(cut);
        }
    }

    return min_cut;
}

void ugraph_dump_to_dot(const ugraph_t *g, const char *name, FILE *out)
{
    UASSERT_INPUT(g);
    UASSERT_INPUT(out);

    const char *arrow = (g->type == UGRAPH_UNDIRECTED) ?  "--" : "->";
    fprintf(out, "%s %s {\n",
           (g->type == UGRAPH_UNDIRECTED) ? "graph" : "digraph", name);
    for (size_t i = 0; i < g->n; i++)
    {
        ulist_t *l = G_AS_PTR(uvector_get_at(g->nodes, i));
        if (!ulist_is_empty(l))
        {
            ulist_iterator_t *li = ulist_iterator_create(l);
            fputs("   ", out);
            while (ulist_iterator_has_next(li))
            {
                ugeneric_t g = ulist_iterator_get_next(li);
                fprintf(out, " %zu %s %zu;", i, arrow, G_AS_SIZE(g));
            }
            fputs("\n", out);
            ulist_iterator_destroy(li);
        }
    }
    fprintf(out, "}\n");
}
