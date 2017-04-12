#include "dsu.h"
#include "mem.h"

struct dsu_opaq {
    size_t rank; // rank
    size_t size; // number of IDs in DSU
    size_t *ids; // IDs array
    size_t *rs;  // rank of subtree rooted at ids[i]
};

dsu_t *dsu_create(size_t size)
{
    ASSERT_INPUT(size);

    dsu_t *d = umalloc(sizeof(*d));
    d->ids = umalloc(size * sizeof(d->ids[0]));
    d->rs = umalloc(size * sizeof(d->rs[0]));
    d->rank = size;
    d->size = size;
    for (size_t i = 0; i < size; i++)
    {
        d->ids[i] = i;
        d->rs[i] = 0;
    }

    return d;
}

static size_t _root(dsu_t *d, size_t e)
{
    while (e != d->ids[e])
    {
        d->ids[e] = d->ids[d->ids[e]]; // path compression
        e = d->ids[e];
    }

    return e;
}

bool dsu_is_united(dsu_t *d, size_t p, size_t q)
{
    ASSERT_INPUT(d);
    ASSERT_INPUT(q < d->size);
    ASSERT_INPUT(p < d->size);

    return _root(d, p) == _root(d, q);
}

void dsu_unite(dsu_t *d, size_t p, size_t q)
{
    ASSERT_INPUT(d);
    ASSERT_INPUT(q < d->size);
    ASSERT_INPUT(p < d->size);

    /* union by rank */
    size_t proot = _root(d, p);
    size_t qroot = _root(d, q);

    if (proot == qroot)
    {
        return;
    }

    if (d->rs[proot] > d->rs[qroot])
    {
        d->ids[qroot] = proot;
    }
    else if (d->rs[proot] < d->rs[qroot])
    {
        d->ids[proot] = qroot;
    }
    else
    {
        d->ids[proot] = qroot;
        d->rs[proot] += 1;
    }
    d->rank -= 1;
}

void dsu_destroy(dsu_t *d)
{
    if (d)
    {
        ufree(d->ids);
        ufree(d->rs);
        ufree(d);
    }
}
