#include "dsu.h"

#include "asserts.h"

int main(void)
{
    udsu_t *d = udsu_create(10);

    udsu_unite(d, 0, 5);
    UASSERT(udsu_is_united(d, 0, 5));
    UASSERT(!udsu_is_united(d, 1, 5));
    udsu_unite(d, 0, 1);
    udsu_unite(d, 1, 2);
    udsu_unite(d, 2, 3);
    UASSERT(udsu_is_united(d, 0, 3));
    udsu_unite(d, 4, 0);
    UASSERT(udsu_is_united(d, 0, 3));
    UASSERT(udsu_is_united(d, 4, 0));
    UASSERT(udsu_is_united(d, 4, 3));
    UASSERT(udsu_is_united(d, 4, 4));
    udsu_destroy(d);
}
