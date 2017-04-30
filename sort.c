#include <string.h>
#include <stdlib.h>
#include "mem.h"
#include "sort.h"

static size_t _insertion_sort(ugeneric_t *base, size_t nmemb, void_cmp_t cmp);
static size_t _partition(ugeneric_t *base, size_t l, size_t r, void_cmp_t cmp);
static void _quick_sort(ugeneric_t *base, size_t l, size_t r, void_cmp_t cmp);
static size_t _merge(ugeneric_t *lbase, size_t lsize, ugeneric_t *rbase,
                     size_t rsize, ugeneric_t *aux, void_cmp_t cmp);
static size_t _merge_sort(ugeneric_t *base, ugeneric_t *aux, size_t nmemb,
                          void_cmp_t cmp);

size_t count_inversions(ugeneric_t *base, size_t nmemb, void_cmp_t cmp)
{
    UASSERT_INPUT(base);

    size_t inv = 0;
    if (nmemb > 1)
    {
        ugeneric_t *aux = umalloc(nmemb * sizeof(*aux));
        inv = _merge_sort(base, aux, nmemb, cmp);
        ufree(aux);
    }

    return inv;
}

void quick_sort(ugeneric_t *base, size_t nmemb, void_cmp_t cmp)
{
    UASSERT_INPUT(base);

    if (nmemb)
    {
        _quick_sort(base, 0, nmemb - 1, cmp);
    }
}

void selection_sort(ugeneric_t *base, size_t nmemb, void_cmp_t cmp)
{
    UASSERT_INPUT(base);
    (void)nmemb;
    (void)cmp;
    UASSERT(0);
}

void merge_sort(ugeneric_t *base, size_t nmemb, void_cmp_t cmp)
{
    UASSERT_INPUT(base);

    if (nmemb > 1)
    {
        ugeneric_t *aux = umalloc(nmemb * sizeof(*aux));
         _merge_sort(base, aux, nmemb, cmp);
        ufree(aux);
    }
}

void insertion_sort(ugeneric_t *base, size_t nmemb, void_cmp_t cmp)
{
    UASSERT_INPUT(base);

    if (nmemb > 1)
    {
        _insertion_sort(base, nmemb, cmp);
    }
}

static void _quick_sort(ugeneric_t *base, size_t l, size_t r, void_cmp_t cmp)
{
    if (r > l)
    {
        size_t p = _partition(base, l, r, cmp);
        _quick_sort(base, l, (p > 0) ? p - 1 : 0, cmp);
        _quick_sort(base, (p < r) ? p + 1 : r, r, cmp);
    }
}

static size_t _partition(ugeneric_t *base, size_t l, size_t r, void_cmp_t cmp)
{
    ugeneric_t p = base[r];
    size_t pi = l;

    for (size_t i = l; i < r; i++)
    {
        if (ugeneric_compare(base[i], p, cmp) <= 0)
        {
            ugeneric_swap(&base[pi], &base[i]);
            pi++;
        }
    }
    ugeneric_swap(&base[pi], &base[r]);

    return pi;
}

static size_t _insertion_sort(ugeneric_t *base, size_t nmemb, void_cmp_t cmp)
{
    size_t inv = 0;

    for (size_t i = 1; i < nmemb; i++)
    {
        int j = i - 1;
        ugeneric_t t = base[i];
        while (j >= 0 && (ugeneric_compare(base[j], t, cmp) > 0))
        {
            base[j + 1] = base[j];
            inv++;
            j--;
        }
        base[j + 1] = t;
    }

    return inv;
}

static size_t _merge(ugeneric_t *lbase, size_t lsize, ugeneric_t *rbase, size_t rsize,
                     ugeneric_t *aux, void_cmp_t cmp)
{
    size_t i, j, k, inv;

    i = j = k = inv = 0;

    while ((i < lsize) && (j < rsize))
    {
        if (ugeneric_compare(lbase[i], rbase[j], cmp) <= 0)
        {
            aux[k++] = lbase[i++];
        }
        else
        {
            aux[k++] = rbase[j++];
            inv += (lsize - i);
        }
    }
    while (i < lsize)
    {
        aux[k++] = lbase[i++];
    }
    while (j < rsize)
    {
        aux[k++] = rbase[j++];
    }

    return inv;
}

static size_t _merge_sort(ugeneric_t *base, ugeneric_t *aux, size_t nmemb, void_cmp_t cmp)
{
    size_t inv = 0;
    size_t j;

    if (nmemb > 1)
    {
        j = nmemb / 2;
        inv += _merge_sort(base, aux, j, cmp);
        inv += _merge_sort(base + j, aux, nmemb - j, cmp);
        inv += _merge(base, j, base + j, nmemb - j, aux, cmp);
        memcpy(base, aux, sizeof(*base) * nmemb);
    }

    return inv;
}
