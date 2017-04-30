#ifndef USORT_H__
#define USORT_H__

#include "generic.h"

void quick_sort(ugeneric_t *base, size_t nmemb, void_cmp_t cmp);
void merge_sort(ugeneric_t *base, size_t nmemb, void_cmp_t cmp);
void insertion_sort(ugeneric_t *base, size_t nmemb, void_cmp_t cmp);
void selection_sort(ugeneric_t *base, size_t nmemb, void_cmp_t cmp);
size_t count_inversions(ugeneric_t *base, size_t nmemb, void_cmp_t cmp);

#endif
