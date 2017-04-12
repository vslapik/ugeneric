#ifndef FILE_UTIL_H__
#define FILE_UTIL_H__

#include <string.h>
#include <errno.h>
#include "generic.h"
#include "string_utils.h"
#include "vector.h"

#define IO_ERROR_MSG "I/O error at %s:%u:%s(): %s."
#define G_ERROR_IO G_ERROR(string_fmt(IO_ERROR_MSG, __FILE__, __LINE__, __func__, strerror(errno)));

generic_t file_open(const char *path, const char *mode);
generic_t file_close(FILE *f);
generic_t file_get_size(const char *path);
generic_t file_read_to_string(const char *path);
generic_t file_read_lines(const char *path);

#endif
