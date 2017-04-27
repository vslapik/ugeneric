#ifndef FILE_UTIL_H__
#define FILE_UTIL_H__

#include <string.h>
#include <errno.h>
#include "generic.h"
#include "string_utils.h"
#include "vector.h"

#define IO_ERROR_MSG "I/O error at %s:%u:%s(): %s."
#define G_ERROR_IO G_ERROR(string_fmt(IO_ERROR_MSG, __FILE__, __LINE__, __func__, strerror(errno)));

typedef struct file_reader_opaq file_reader_t;
typedef struct file_writer_opaq file_writer_t;

generic_t file_open(const char *path, const char *mode);
generic_t file_close(FILE *f);
generic_t file_get_size(const char *path);
generic_t file_read_to_string(const char *path);
generic_t file_read_lines(const char *path);

generic_t file_reader_create(const char *path, size_t buffer_size);
generic_t file_reader_read(file_reader_t *fr, size_t size, memchunk_t *chun);
generic_t file_reader_get_file_size(file_reader_t *fr);
generic_t file_reader_get_position(const file_reader_t *fr);
generic_t file_reader_set_position(file_reader_t *fr, size_t position);
generic_t file_reader_reset(file_reader_t *fr);
size_t file_reader_get_buffer_size(const file_reader_t *fr);
bool file_reader_has_next(const file_reader_t *fr);
FILE *file_reader_get_file(const file_reader_t *fr);
generic_t file_reader_destroy(file_reader_t *fr);

generic_t file_writer_create(const char *path);
generic_t file_writer_write(file_writer_t *fw, memchunk_t mchunk);
generic_t file_writer_get_file_size(file_writer_t *fw);
generic_t file_writer_get_position(const file_writer_t *fw);
generic_t file_writer_set_position(file_writer_t *fw, size_t position);
FILE *file_writer_get_file(const file_writer_t *fw);
generic_t file_writer_destroy(file_writer_t *fw);

#endif
