#include <stdio.h>
#include <stdlib.h>
#include "mem.h"
#include "file_utils.h"

generic_t file_get_size(const char *path)
{
    ASSERT_INPUT(path);

    generic_t g = file_open(path, "r");
    if (G_IS_ERROR(g))
    {
        return g;
    }

    FILE *f = G_AS_PTR(g);
    if (fseek(f, 0, SEEK_END) != 0)
    {
        return G_ERROR_IO;
    }

    long fsize = ftell(f);
    if (fsize == -1)
    {
        return G_ERROR_IO;
    }

    if (fclose(f) != 0)
    {
        return G_ERROR_IO;
    }

    return G_SIZE(fsize);
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
    if (0 != fclose(f))
    {
        return G_ERROR_IO;
    }

    return G_NULL;
}
