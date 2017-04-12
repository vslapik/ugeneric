#include "dsu.h"

int main(void)
{
    dsu_t *d = dsu_create(10);

    dsu_unite(d, 0, 5);
    ASSERT(dsu_is_united(d, 0, 5));
    ASSERT(!dsu_is_united(d, 1, 5));
    dsu_unite(d, 0, 1);
    dsu_unite(d, 1, 2);
    dsu_unite(d, 2, 3);
    ASSERT(dsu_is_united(d, 0, 3));
    dsu_unite(d, 4, 0);
    ASSERT(dsu_is_united(d, 0, 3));
    ASSERT(dsu_is_united(d, 4, 0));
    ASSERT(dsu_is_united(d, 4, 3));
    ASSERT(dsu_is_united(d, 4, 4));
    dsu_destroy(d);
}
