#ifndef SORT_H__
#define SORT_H__

#include "generic.h"

void quick_sort(generic_t *base, size_t nmemb, void_cmp_t cmp);
void merge_sort(generic_t *base, size_t nmemb, void_cmp_t cmp);
void insertion_sort(generic_t *base, size_t nmemb, void_cmp_t cmp);
void selection_sort(generic_t *base, size_t nmemb, void_cmp_t cmp);
size_t count_inversions(generic_t *base, size_t nmemb, void_cmp_t cmp);

#endif
