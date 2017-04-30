#ifndef USTRING_UTILS_H__
#define USTRING_UTILS_H__

#include "vector.h"
#include "mem.h"

uvector_t *ustring_split(const char *str, const char *sep);
char *ustring_dup(const char *str);
char *ustring_ndup(const char *str, size_t n);
char *ustring_fmt(const char *fmt, ...);
umemchunk_t ustring_fmt_to_memchunk(const char *fmt, ...);
bool ustring_starts_with(const char *str, const char *prefix);

#endif
