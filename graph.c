#include "graph.h"

#include "bitmap.h"
#include "dsu.h"
#include "list.h"
#include "mem.h"
#include "queue.h"
#include "stack.h"
#include "vector.h"

struct ugraph_opaq {
    uvector_t *adj; // adj lists
    ugraph_type_t type;
    size_t m; // edges
    size_t n; // nodes
};

struct ugraph_edge_iterator_opaq {
    const ugraph_t *g;
    ulist_iterator_t *li;
};

// This function is used for searching the edge from source vertex to sink vertex,
// thus it compares only these two field of edge structure, weight is unrelevant.
int _edge_cmp(const void *ptr1, const void *ptr2)
{
    const ugraph_edge_t *e1 = ptr1;
    const ugraph_edge_t *e2 = ptr2;

    if (e1->f < e2->f) return -1;
    if (e1->f > e2->f) return  1;
    if (e1->t < e2->t) return -1;
    if (e1->t > e2->t) return  1;

    return  0;
}

ugraph_t *ugraph_create(size_t n, ugraph_type_t type)
{
    ugraph_t *g = umalloc(sizeof(*g));
    g->adj = uvector_create_with_size(n, G_PTR(NULL));
    uvector_set_void_destroyer(g->adj, (void_dtr_t)ulist_destroy);
    g->n = n;
    g->m = 0;
    g->type = type;

    for (size_t i = 0; i < g->n; i++)
    {
        ulist_t *l = ulist_create();
        uvector_set_at(g->adj, i, G_PTR(l));
        ulist_set_void_comparator(l, _edge_cmp);
        ulist_set_void_destroyer(l, ufree);
    }

    return g;
}

static inline void swap(size_t *x, size_t *y)
{
    size_t tmp = *x;
    *x = *y;
    *y = tmp;
}

const ugraph_edge_t *ugraph_get_edge(const ugraph_t *g, size_t from, size_t to)
{
    UASSERT_INPUT(g);
    UASSERT_INPUT(from < g->n);
    UASSERT_INPUT(to < g->n);

    ugraph_edge_t tmp = (ugraph_edge_t){.f = from, .t = to, .w = 0};

    ulist_t *adj = G_AS_PTR(uvector_get_at(g->adj, from));
    ugeneric_t *edge = ulist_find(adj, G_PTR(&tmp));

    return (edge) ? G_AS_PTR(*edge) : NULL;
}

void ugraph_add_edge(ugraph_t *g, size_t from, size_t to, int weight)
{
    UASSERT_INPUT(g);
    UASSERT_INPUT(from < g->n);
    UASSERT_INPUT(to < g->n);

    const ugraph_edge_t *edge = ugraph_get_edge(g, from, to);
    if (!edge)
    {
        ugraph_edge_t *e = umalloc(sizeof(*edge));
        e->f = from;
        e->t = to;
        e->w = weight;

        ulist_append(G_AS_PTR(uvector_get_at(g->adj, from)), G_PTR(e));

        if ((g->type == UGRAPH_UNDIRECTED))
        {
            e = umalloc(sizeof(*edge));
            e->f = to;
            e->t = from;
            e->w = weight;
            // For undirected graph store two edges (backward and forward)
            // in order to make BFS and DFS work.
            ulist_append(G_AS_PTR(uvector_get_at(g->adj, to)), G_PTR(e));
        }

        g->m += 1;
    }
    else
    {
        ((ugraph_edge_t *)edge)->w = weight;
        if ((g->type == UGRAPH_UNDIRECTED))
        {
            // Update the counterpart as well.
            edge = ugraph_get_edge(g, to, from);
            UASSERT_INTERNAL(edge);
            ((ugraph_edge_t *)edge)->w = weight;
        }
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
        uvector_destroy(g->adj);
        ufree(g);
    }
}

uvector_t *ugraph_get_edges(const ugraph_t *g)
{
    UASSERT_INPUT(g);

    const ugraph_edge_t *e = NULL;
    ugraph_edge_iterator_t *ei = NULL;
    uvector_t *v = uvector_create();
    uvector_reserve_capacity(v, g->m);
    uvector_set_void_destroyer(v, ufree);

    for (size_t i = 0; i < g->n; i++)
    {
        ei = ugraph_edge_iterator_create(g, i);
        while (ugraph_edge_iterator_has_next(ei))
        {
            e = ugraph_edge_iterator_get_next(ei);
            if (g->type == UGRAPH_UNDIRECTED)
            {
                // As internally there are two edges (forward and backward)
                // in undirected graph we return only the first one (f -> t)
                if (e->t < i)
                {
                    continue;
                }
            }
            ugraph_edge_t *edge = umalloc(sizeof(*edge));
            edge->f = i;
            edge->t = e->t;
            edge->w = e->w;
            uvector_append(v, G_PTR(edge));
        }
        ugraph_edge_iterator_destroy(ei);
    }

    UASSERT_INTERNAL(uvector_get_size(v) == g->m);

    return v;
}

static uvector_t *_min_cut(const ugraph_t *g)
{
    uvector_t *edges = ugraph_get_edges(g); // owner of data
    uvector_t *edgestmp = uvector_copy(edges); // shallow copy
    size_t edges_count = ugraph_get_edge_count(g);
    size_t vertex_count = ugraph_get_vertex_count(g);

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
    uvector_set_void_destroyer(mincut, ufree);
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

void ugraph_bfs(const ugraph_t *g, size_t root, ugraph_node_callback_t cb, void *data)
{
    UASSERT_INPUT(g);
    UASSERT_INPUT(root < g->n);

    const ugraph_edge_t *e = NULL;
    ugraph_edge_iterator_t *ei = NULL;
    uqueue_t *q = uqueue_create();
    uint8_t *seen_nodes = ubitmap_allocate(g->n);

    uqueue_enq(q, G_INT(root));
    ubitmap_set_bit(seen_nodes, root);

    while (!uqueue_is_empty(q))
    {
        // Extract next node.
        size_t node = G_AS_INT(uqueue_deq(q));

        // Run callback on it.
        if (cb && (cb(g, node, data)))
        {
            goto exit;
        }
        // Put all current node non-visited neighbours to the queue.
        ei = ugraph_edge_iterator_create(g, node);
        while (ugraph_edge_iterator_has_next(ei))
        {
            e = ugraph_edge_iterator_get_next(ei);
            if (!ubitmap_bit_is_set(seen_nodes, e->t))
            {
                uqueue_enq(q, G_INT(e->t));
                ubitmap_set_bit(seen_nodes, e->t);
            }
        }
        ugraph_edge_iterator_destroy(ei);
    }

exit:
    ufree(seen_nodes);
    uqueue_destroy(q);
}

void ugraph_dfs(const ugraph_t *g, size_t root, ugraph_node_callback_t cb, void *data)
{
    UASSERT_INPUT(g);
    UASSERT_INPUT(root < g->n);

    const ugraph_edge_t *e = NULL;
    ugraph_edge_iterator_t *ei = NULL;
    ustack_t *s = ustack_create();
    uint8_t *visited_nodes = ubitmap_allocate(g->n);

    ustack_push(s, G_INT(root));
    while (!ustack_is_empty(s))
    {
        size_t node = G_AS_INT(ustack_peek(s));

        // Run callback on it.
        if (!ubitmap_bit_is_set(visited_nodes, node))
        {
            ubitmap_set_bit(visited_nodes, node);
            if (cb && (cb(g, node, data)))
            {
                goto exit;
            }
        }

        bool backtrack = true;
        ei = ugraph_edge_iterator_create(g, node);
        while (ugraph_edge_iterator_has_next(ei))
        {
            e = ugraph_edge_iterator_get_next(ei);
            if (!ubitmap_bit_is_set(visited_nodes, e->t))
            {
                ustack_push(s, G_INT(e->t));
                backtrack = false;
                break;
            }
        }
        ugraph_edge_iterator_destroy(ei);

        if (backtrack)
        {
            ustack_pop(s);
        }
    }

exit:
    ufree(visited_nodes);
    ustack_destroy(s);
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
        ugraph_edge_iterator_t *ei = ugraph_edge_iterator_create(g, i);
        if (ugraph_edge_iterator_has_next(ei))
        {
            fputs("   ", out);
            while (ugraph_edge_iterator_has_next(ei))
            {
                const ugraph_edge_t *e = ugraph_edge_iterator_get_next(ei);
                fprintf(out, " %zu %s %zu;", i, arrow, e->t);
            }
            fputs("\n", out);
        }
        ugraph_edge_iterator_destroy(ei);
    }
    fprintf(out, "}\n");
}

ugraph_edge_iterator_t *ugraph_edge_iterator_create(const ugraph_t *g, size_t n)
{
    UASSERT_INPUT(g);
    UASSERT_INPUT(n < g->n);

    ugraph_edge_iterator_t *ei = umalloc(sizeof(*ei));
    ulist_t *l = G_AS_PTR(uvector_get_at(g->adj, n));
    ei->li = ulist_iterator_create(l);

    return ei;
}

const ugraph_edge_t *ugraph_edge_iterator_get_next(ugraph_edge_iterator_t *ei)
{
    UASSERT_INPUT(ei);
    ugeneric_t edge_ptr = ulist_iterator_get_next(ei->li);
    return (ugraph_edge_t *)G_AS_PTR(edge_ptr);
}

bool ugraph_edge_iterator_has_next(const ugraph_edge_iterator_t *ei)
{
    UASSERT_INPUT(ei);
    return ulist_iterator_has_next(ei->li);
}

void ugraph_edge_iterator_reset(ugraph_edge_iterator_t *ei)
{
    UASSERT_INPUT(ei);
    ulist_iterator_reset(ei->li);
}

void ugraph_edge_iterator_destroy(ugraph_edge_iterator_t *ei)
{
    if (ei)
    {
        ulist_iterator_destroy(ei->li);
        ufree(ei);
    }
}
