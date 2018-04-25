#ifndef UFILE_UTILS_H__
#define UFILE_UTILS_H__

#include "generic.h"
#include "string_utils.h"
#include "vector.h"
#include <errno.h>

#define IO_ERROR_MSG "I/O error at %s:%u:%s(): %s."
#define G_ERROR_IO G_ERROR(ustring_fmt(IO_ERROR_MSG, __FILE__, __LINE__, __func__, strerror(errno)))

typedef struct ufile_reader_opaq ufile_reader_t;
typedef struct ufile_writer_opaq ufile_writer_t;

typedef ugeneric_t (*ufile_error_handler_t)(ugeneric_t io_error, void *ctx);
void libugeneric_set_file_error_handler(ufile_error_handler_t error_handler,
                                        void *error_handler_ctx);

ugeneric_t ufile_open(const char *path, const char *mode);
ugeneric_t ufile_close(FILE *f);
ugeneric_t ufile_get_size(const char *path);
ugeneric_t ufile_read_to_string(const char *path);
ugeneric_t ufile_read_to_memchunk(const char *path);
ugeneric_t ufile_read_lines(const char *path);
ugeneric_t ufile_create_from_memchunk(const char *path, umemchunk_t mchunk);

ugeneric_t ufile_reader_create(const char *path, size_t buffer_size);
ugeneric_t ufile_reader_read(ufile_reader_t *fr, size_t size, void *buffer);
ugeneric_t ufile_reader_get_file_size(ufile_reader_t *fr);
ugeneric_t ufile_reader_get_position(const ufile_reader_t *fr);
ugeneric_t ufile_reader_set_position(ufile_reader_t *fr, size_t position);
ugeneric_t ufile_reader_reset(ufile_reader_t *fr);
size_t ufile_reader_get_buffer_size(const ufile_reader_t *fr);
bool ufile_reader_has_next(const ufile_reader_t *fr);
ugeneric_t ufile_reader_destroy(ufile_reader_t *fr);

ugeneric_t ufile_writer_create(const char *path);
ugeneric_t ufile_writer_write(ufile_writer_t *fw, umemchunk_t mchunk);
ugeneric_t ufile_writer_get_file_size(ufile_writer_t *fw);
ugeneric_t ufile_writer_get_position(const ufile_writer_t *fw);
ugeneric_t ufile_writer_set_position(ufile_writer_t *fw, size_t position);
ugeneric_t ufile_writer_destroy(ufile_writer_t *fw);

#endif
