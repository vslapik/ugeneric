#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "mem.h"
#include "buffer.h"
#include "file_utils.h"

struct file_reader_opaq {
    FILE *file;
    size_t file_size;
    size_t read_offset;
    void *buffer;
    size_t buffer_size;
};

struct file_writer_opaq {
    FILE *file;
};

static generic_t _get_position(FILE *f)
{
    long pos = ftell(f);
    if (pos == -1)
    {
        return G_ERROR_IO;
    }

    return G_SIZE(pos);
}

static generic_t _set_position(FILE *f, size_t position)
{
    if (fseek(f, position, SEEK_SET) == -1)
    {
        return G_ERROR_IO;
    }
    return G_NULL;
}

static generic_t _get_file_size(FILE *f, bool save_pos)
{
    long saved_pos;
    long fsize;
    generic_t g;

    if (save_pos)
    {
        if (G_IS_ERROR(g = _get_position(f)))
        {
            return g;
        }
        saved_pos = G_AS_SIZE(g);
    }

    if (fseek(f, 0, SEEK_END) == -1)
    {
        return G_ERROR_IO;
    }

    if (G_IS_ERROR(g = _get_position(f)))
    {
        return g;
    }
    fsize = G_AS_SIZE(g);

    if (save_pos)
    {
        if (G_IS_ERROR(g = _set_position(f, saved_pos)))
        {
            return G_ERROR_IO;
        }
    }

    return G_SIZE(fsize);
}

generic_t file_get_size(const char *path)
{
    ASSERT_INPUT(path);

    generic_t g = file_open(path, "r");
    if (G_IS_ERROR(g))
    {
        return g;
    }

    FILE *f = G_AS_PTR(g);
    g = _get_file_size(f, false);

    if (fclose(f) != 0)
    {
        return G_ERROR_IO;
    }

    return g;
}

generic_t file_read_to_string(const char *path)
{
    ASSERT_INPUT(path);

    generic_t g = file_get_size(path);
    if (G_IS_ERROR(g))
    {
        return g;
    }
    size_t fsize = G_AS_SIZE(g);

    g = file_open(path, "r");
    if (G_IS_ERROR(g))
    {
        return g;
    }

    FILE *f = G_AS_PTR(g);
    char *t = umalloc(fsize + 1);
    if (fread(t, 1, fsize, f) < fsize)
    {
        return G_ERROR_IO;
    }
    t[fsize] = 0;

    if (fclose(f) != 0)
    {
        return G_ERROR_IO;
    }

    return G_STR(t);
}

generic_t file_read_lines(const char *path)
{
    ASSERT_INPUT(path);

    generic_t g = file_read_to_string(path);
    if (G_IS_ERROR(g))
    {
        return g;
    }

    char *str = G_AS_STR(g);
    vector_t *v = string_split(str, "\n");
    ufree(str);

    if (strlen(G_AS_STR(vector_get_back(v))) == 0)
    {
        /*  Remove an empty string from the end of the vector
         *  which appears after split when the last line ends
         *  with \n which is usually true.
         */
        ufree(G_AS_STR(vector_pop_back(v)));
    }

    return G_PTR(v);
}

generic_t file_open(const char *path, const char *mode)
{
    ASSERT_INPUT(path);
    ASSERT_INPUT(mode);

    FILE *file = fopen(path, mode);
    if (!file)
    {
        return G_ERROR_IO;
    }

    return G_PTR(file);
}

generic_t file_close(FILE *f)
{
    ASSERT_INPUT(f);

    if (0 != fclose(f))
    {
        return G_ERROR_IO;
    }

    return G_NULL;
}

generic_t file_reader_create(const char *path, size_t buffer_size)
{
    ASSERT_INPUT(path);
    ASSERT_INPUT(buffer_size);

    generic_t g;
    if (G_IS_ERROR(g = file_open(path, "r")))
    {
        return g;
    }
    file_reader_t *fr = umalloc(sizeof(*fr));
    fr->buffer = umalloc(buffer_size);
    fr->buffer_size = buffer_size;
    fr->file = G_AS_PTR(g);
    fr->read_offset = 0;

    if (G_IS_ERROR(g = _get_file_size(fr->file, true)))
    {
        return g;
    }

    fr->file_size = G_AS_SIZE(g);

    return G_PTR(fr);
}

generic_t file_reader_read_full_buffer(file_reader_t *fr)
{
    return file_reader_read(fr, fr->buffer_size);
}

generic_t file_reader_read(file_reader_t *fr, size_t size)
{
    ASSERT_INPUT(fr);

    // Expand buffer if it is not sufficient.
    if (fr->buffer_size < size)
    {
        ufree(fr->buffer);
        fr->buffer = umalloc(size);
        fr->buffer_size = size;
    }

    size_t r = fread(fr->buffer, 1, size, fr->file);
    fr->read_offset += r;

    // Short read, either EOF reached or I/O error.
    if (r < size)
    {
        if (!feof(fr->file))
        {
            return G_ERROR_IO;
        }
    }

    return G_MEMCHUNK(fr->buffer, r);
}

bool file_reader_has_next(const file_reader_t *fr)
{
    return fr->read_offset < fr->file_size;
}

generic_t file_reader_reset(file_reader_t *fr)
{
    ASSERT_INPUT(fr);

    if (fseek(fr->file, 0, SEEK_SET) == -1)
    {
        return G_ERROR_IO;
    }
    fr->read_offset = 0;

    return G_NULL;
}

generic_t file_reader_get_file_size(file_reader_t *fr)
{
    ASSERT_INPUT(fr);
    return _get_file_size(fr->file, true);
}

size_t file_reader_get_buffer_size(const file_reader_t *fr)
{
    ASSERT_INPUT(fr);
    return fr->buffer_size;
}

generic_t file_reader_get_position(const file_reader_t *fr)
{
    ASSERT_INPUT(fr);
    return _get_position(fr->file);
}

generic_t file_reader_set_position(file_reader_t *fr, size_t position)
{
    ASSERT_INPUT(fr);
    return _set_position(fr->file, position);
}

FILE *file_reader_get_file(const file_reader_t *fr)
{
    return fr->file;
}

generic_t file_reader_destroy(file_reader_t *fr)
{
    generic_t g = G_NULL;
    if (fr)
    {
        g = file_close(fr->file);
        ufree(fr->buffer);
        ufree(fr);
    }

    return g;
}

generic_t file_writer_create(const char *path)
{
    ASSERT_INPUT(path);

    generic_t g;
    if (G_IS_ERROR(g = file_open(path, "w")))
    {
        return g;
    }
    file_writer_t *fw = umalloc(sizeof(*fw));
    fw->file = G_AS_PTR(g);

    return G_PTR(fw);
}

generic_t file_writer_write(file_writer_t *fw, memchunk_t mchunk)
{
    ASSERT_INPUT(fw);

    size_t size = fwrite(mchunk.data, 1, mchunk.size, fw->file);

    // Short write, either EOF reached or I/O error.
    if (size < mchunk.size)
    {
        return G_ERROR_IO;
    }

    return G_NULL;
}

generic_t file_writer_get_file_size(file_writer_t *fw)
{
    ASSERT_INPUT(fw);
    return _get_file_size(fw->file, true);
}

generic_t file_writer_get_position(const file_writer_t *fw)
{
    ASSERT_INPUT(fw);
    return _get_position(fw->file);
}

generic_t file_writer_set_position(file_writer_t *fw, size_t position)
{
    ASSERT_INPUT(fw);
    return _set_position(fw->file, position);
}

FILE *file_writer_get_file(const file_writer_t *fw)
{
    return fw->file;
}

generic_t file_writer_destroy(file_writer_t *fw)
{
    generic_t g = G_NULL;
    if (fw)
    {
        g = file_close(fw->file);
        ufree(fw);
    }

    return g;
}
