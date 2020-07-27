#include "dsu.h"
#include "mem.h"

struct udsu_opaq {
    size_t rank; // rank
    size_t size; // number of IDs in DSU
    size_t *ids; // IDs array

    // Upper-bound of rank of subtree rooted at ids[i],
    // path, path-compression doesn't updated it makes it
    // out of think with subtree depth which is expected
    // size effect (see CLRS).
    size_t *rs;
};

udsu_t *udsu_create(size_t size)
{
    UASSERT_INPUT(size);

    udsu_t *d = umalloc(sizeof(*d));
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

static size_t _root(const udsu_t *d, size_t e)
{
    while (e != d->ids[e])
    {
        d->ids[e] = d->ids[d->ids[e]]; // path compression
        e = d->ids[e];
    }

    return e;
}

bool udsu_is_united(const udsu_t *d, size_t p, size_t q)
{
    UASSERT_INPUT(d);
    UASSERT_INPUT(q < d->size);
    UASSERT_INPUT(p < d->size);

    return _root(d, p) == _root(d, q);
}

size_t udsu_get_rank(const udsu_t *d)
{
    return d->rank;
}

void udsu_unite(udsu_t *d, size_t p, size_t q)
{
    UASSERT_INPUT(d);
    UASSERT_INPUT(q < d->size);
    UASSERT_INPUT(p < d->size);

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

void udsu_destroy(udsu_t *d)
{
    if (d)
    {
        ufree(d->ids);
        ufree(d->rs);
        ufree(d);
    }
}
