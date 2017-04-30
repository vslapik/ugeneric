#ifndef UBITMAP_H__
#define UBITMAP_H__

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#include "mem.h"

/* MSB0 bit numbering, rfc1166 */
static inline void *ubitmap_allocate(size_t len)               { return ucalloc(len / 8 + (bool)(len % 8), 1);               }
static inline bool  ubitmap_get_bit(const void *a, size_t num) { return ((uint8_t *)a)[(num) / 8] &   (0x80 >> ((num) % 8)); }
static inline void  ubitmap_set_bit(void *a, size_t num)       {        ((uint8_t *)a)[(num) / 8] |=  (0x80 >> ((num) % 8)); }
static inline void  ubitmap_clear_bit(void *a, size_t num)     {        ((uint8_t *)a)[(num) / 8] &= ~(0x80 >> ((num) % 8)); }
static inline void  ubitmap_flip_bit(void *a, size_t num)      {        ((uint8_t *)a)[(num) / 8] ^=  (0x80 >> ((num) % 8)); }

void ubitmap_set_range(void *a, size_t l, size_t r);
void ubitmap_flip_all(void *b, size_t len);
void ubitmap_flip_range(void *b, size_t l, size_t r);

char *ubitmap_range_as_str(const void *b, size_t l, size_t r);
char *ubitmap_as_str(const void *a, size_t len);
int ubitmap_fprint_range(const void *a, size_t l, size_t r, FILE *f);
int ubitmap_fprint(const void *a, size_t len, FILE *f);

/*
void ubitmap_swap_bits(void *a, size_t i1, size_t i2);
void ubitmap_swap_ranges(bitmap_t *b, size_t l1, size_t r1,
                        size_t l2, size_t r2);
void ubitmap_reverse_range(bitmap_t *b, size_t l, size_t r);
void ubitmap_reverse(bitmap_t *b);
*/

#endif
