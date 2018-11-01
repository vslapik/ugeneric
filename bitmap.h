#ifndef UBITMAP_H__
#define UBITMAP_H__

#include "mem.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

static inline size_t ubitmap_get_octets_len(size_t bit_len)       {return bit_len / 8 + (bool)(bit_len % 8);}

/* MSB0 bit numbering, rfc1166 */
static inline void *ubitmap_allocate(size_t len)                  {return ucalloc(ubitmap_get_octets_len(len), 1);            }
static inline void  ubitmap_set_bit(void *a, size_t num)          {       ((uint8_t *)a)[(num) / 8] |=  (0x80 >> ((num) % 8));}
static inline void  ubitmap_clear_bit(void *a, size_t num)        {       ((uint8_t *)a)[(num) / 8] &= ~(0x80 >> ((num) % 8));}
static inline bool  ubitmap_bit_is_set(const void *a, size_t num) {return ((uint8_t *)a)[(num) / 8] &   (0x80 >> ((num) % 8));}
static inline void  ubitmap_flip_bit(void *a, size_t num)         {       ((uint8_t *)a)[(num) / 8] ^=  (0x80 >> ((num) % 8));}

void ubitmap_flip_all(void *b, size_t len);
void ubitmap_flip_range(void *b, size_t l, size_t r);

static inline void ubitmap_set_all(void *bmp, size_t len)        {memset(bmp, 1, ubitmap_get_octets_len(len));}
static inline void ubitmap_clear_all(void *bmp, size_t len)      {memset(bmp, 0, ubitmap_get_octets_len(len));}

char *ubitmap_range_as_str(const void *b, size_t l, size_t r);
char *ubitmap_as_str(const void *a, size_t len);
int ubitmap_fprint_range(const void *a, size_t l, size_t r, FILE *f);
int ubitmap_fprint(const void *a, size_t len, FILE *f);
static inline int ubitmap_print(const void *a, size_t len)               {return ubitmap_fprint(a, len, stdout);}
static inline int ubitmap_print_range(const void *a, size_t l, size_t r) {return ubitmap_fprint_range(a, l, r, stdout);}

/*
void ubitmap_set_range(void *a, size_t l, size_t r);
void ubitmap_clear_range(void *a, size_t l, size_t r);
void ubitmap_swap_bits(void *a, size_t i1, size_t i2);
void ubitmap_swap_ranges(void *a, size_t l1, size_t r1,
                        size_t l2, size_t r2);
void ubitmap_reverse_range(void *a, size_t l, size_t r);
void ubitmap_reverse(void *a);
*/

#endif
