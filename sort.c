#include <string.h>
#include <stdlib.h>
#include "mem.h"
#include "sort.h"

static size_t _insertion_sort(ugeneric_t *base, size_t nmemb, void_cmp_t cmp);
static size_t _merge(ugeneric_t *lbase, size_t lsize, ugeneric_t *rbase,
                     size_t rsize, ugeneric_t *aux, void_cmp_t cmp);
static size_t _merge_sort(ugeneric_t *base, ugeneric_t *aux, size_t nmemb,
                          void_cmp_t cmp);

/* [l:r) */
/* Hoar partitioning separates array into [smaller|larger or equal]
 * subarrays, range indexes in invocation of quick sort recursive
 * call should be passed accordingly. Another partitioning method
 * (say, Lomuto) may partition array in slightly different way and
 * improper invocation of quicksort will lead to mistakes difficult to
 * spot and debug.
 */
static size_t _hoar_partition(ugeneric_t *base, size_t l, size_t r, void_cmp_t cmp)
{
    size_t i = l;
    size_t j = r - 1;
    size_t m = l + (r - l) / 2;

    /* Choosing a good pivot is a black magic, use median-of-three pivoting
     * here, attributes to Robert Sedgewick.
     */
    if (ugeneric_compare(base[m], base[i], cmp) < 0)
    {
        ugeneric_swap(base + m, base + i);
    }
    if (ugeneric_compare(base[j], base[m], cmp) < 0)
    {
        ugeneric_swap(base + j, base + m);
        if (ugeneric_compare(base[m], base[i], cmp) < 0)
        {
            ugeneric_swap(base + m, base + i);
        }
    }
    ugeneric_t p = base[m];

    for (;;)
    {
        /* Collapsing walls. */
        do { i++; } while (ugeneric_compare(p, base[i], cmp) > 0);
        do { j--; } while (ugeneric_compare(p, base[j], cmp) < 0);

        if (i >= j)
        {
            return j;
        }
        else
        {
            ugeneric_swap(base + i, base + j);
        }
    }
}

/* [l:r) */
static void _quick_sort(ugeneric_t *base, size_t l, size_t r, void_cmp_t cmp)
{
    if (r - l > 1) // terminate if [l, r) defines a single element
    {
        size_t pi = _hoar_partition(base, l, r, cmp);

        _quick_sort(base, l, pi + 1, cmp);
        _quick_sort(base, pi + 1, r, cmp);
    }
}

static size_t _insertion_sort(ugeneric_t *base, size_t nmemb, void_cmp_t cmp)
{
    size_t inv = 0;

    for (size_t i = 1; i < nmemb; i++)
    {
        size_t j = i;
        ugeneric_t t = base[i];
        while (j > 0 && (ugeneric_compare(base[j - 1], t, cmp) > 0))
        {
            base[j] = base[j - 1];
            inv++;
            j--;
        }
        base[j] = t;
    }

    return inv;
}

static size_t _merge(ugeneric_t *lbase, size_t lsize, ugeneric_t *rbase,
                     size_t rsize, ugeneric_t *aux, void_cmp_t cmp)
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
    while (i < lsize) { aux[k++] = lbase[i++]; }
    while (j < rsize) { aux[k++] = rbase[j++]; }

    return inv;
}

static size_t _merge_sort(ugeneric_t *base, ugeneric_t *aux, size_t nmemb,
                          void_cmp_t cmp)
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

static void _hsort(ugeneric_t *base, size_t l, size_t r, void_cmp_t cmp)
{
    /* Uses quick sort as base algorithm, switches to insertion
     * sort on small arrays which is expected to be faster.
     */
    if (r - l > 1) // terminate if [l, r) defines a single element
    {
        if ((r - l) > USORT_HYBRID_THRESHOLD)
        {
            size_t pi = _hoar_partition(base, l, r, cmp);
            _hsort(base, l, pi + 1, cmp);
            _hsort(base, pi + 1, r, cmp);
        }
        else
        {
            _insertion_sort(base + l, r - l, cmp);
        }
    }
}

size_t count_inversions(ugeneric_t *base, size_t nmemb, void_cmp_t cmp)
{

    size_t inv = 0;
    if (nmemb > 1)
    {
        UASSERT_INPUT(base);
        ugeneric_t *aux = umalloc(nmemb * sizeof(*aux));
        inv = _merge_sort(base, aux, nmemb, cmp);
        ufree(aux);
    }

    return inv;
}

void quick_sort(ugeneric_t *base, size_t nmemb, void_cmp_t cmp)
{
    if (nmemb > 1)
    {
        UASSERT_INPUT(base);
        _quick_sort(base, 0, nmemb, cmp);
    }
}

void selection_sort(ugeneric_t *base, size_t nmemb, void_cmp_t cmp)
{
    if (nmemb > 1)
    {
        UASSERT_INPUT(base);
        for (size_t i = 0; i < nmemb - 1; i++) // the very last element intentionally omitted
        {
            size_t min = i;
            for (size_t j = i + 1; j < nmemb; j++)
            {
                if (ugeneric_compare(base[min], base[j], cmp) > 0)
                {
                    min = j;
                }
            }
            ugeneric_swap(base + min, base + i);
        }
    }
}

void merge_sort(ugeneric_t *base, size_t nmemb, void_cmp_t cmp)
{
    if (nmemb > 1)
    {
        UASSERT_INPUT(base);
        ugeneric_t *aux = umalloc(nmemb * sizeof(*aux));
         _merge_sort(base, aux, nmemb, cmp);
        ufree(aux);
    }
}

void insertion_sort(ugeneric_t *base, size_t nmemb, void_cmp_t cmp)
{
    if (nmemb > 1)
    {
        UASSERT_INPUT(base);
        _insertion_sort(base, nmemb, cmp);
    }
}

void hybrid_sort(ugeneric_t *base, size_t nmemb, void_cmp_t cmp)
{
    if (nmemb > 1)
    {
        UASSERT_INPUT(base);
        _hsort(base, 0, nmemb, cmp);
    }
}
