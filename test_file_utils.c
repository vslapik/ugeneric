#include "file_utils.h"
#include "mem.h"
#include "ut_utils.h"

size_t execute_read(const char *path, size_t buffer_size)
{
    size_t read_count = 0;
    ugeneric_t g = ufile_reader_create(path, buffer_size);
    UASSERT_NO_ERROR(g);

    ufile_reader_t *fr = G_AS_PTR(g);
    size_t offset = 0;
    while (ufile_reader_has_next(fr))
    {
        g = ufile_reader_get_position(fr); UASSERT_NO_ERROR(g);
        UASSERT_INT_EQ(offset, G_AS_SIZE(g));
        g = ufile_reader_read(fr, buffer_size, NULL); UASSERT_NO_ERROR(g);
        offset += buffer_size;

//        ugeneric_print(g);
        read_count++;
    }

    ufile_reader_destroy(fr);

    return read_count;
}

void test_ufile_reader(void)
{
    ugeneric_t g;
    umemchunk_t m;
    char *data;
    ufile_reader_t *fr;

    size_t buffer_size = 3;
    UASSERT_NO_ERROR(g = ufile_reader_create("utdata/100", buffer_size));
    fr = G_AS_PTR(g);

    UASSERT_NO_ERROR(g = ufile_reader_read(fr, buffer_size, NULL));
    m = G_AS_MEMCHUNK(g);
    data = umemchunk_as_str(m);
    UASSERT_STR_EQ(data, "313233");
    ufree(data);

    UASSERT_NO_ERROR(g = ufile_reader_read(fr, buffer_size, NULL));
    m = G_AS_MEMCHUNK(g);
    data = umemchunk_as_str(m);
    UASSERT_STR_EQ(data, "343536");
    ufree(data);

    UASSERT_NO_ERROR(g = ufile_reader_read(fr, buffer_size, NULL));
    m = G_AS_MEMCHUNK(g);
    data = umemchunk_as_str(m);
    UASSERT_STR_EQ(data, "373839");
    ufree(data);

    UASSERT_NO_ERROR(ufile_reader_set_position(fr, 0));

    UASSERT_NO_ERROR(g = ufile_reader_read(fr, buffer_size, NULL));
    m = G_AS_MEMCHUNK(g);
    data = umemchunk_as_str(m);
    UASSERT_STR_EQ(data, "313233");
    ufree(data);

    ufile_reader_destroy(fr);

    //
    UASSERT_INT_EQ(execute_read("utdata/100", 1), 100);
    UASSERT_INT_EQ(execute_read("utdata/100", 11), 10);
    UASSERT_INT_EQ(execute_read("utdata/100", 1), 100);
    UASSERT_INT_EQ(execute_read("utdata/empty", 1), 0);
    UASSERT_INT_EQ(execute_read("utdata/empty", 1001), 0);

    UASSERT_NO_ERROR(g = ufile_reader_create("utdata/empty", 10));
    fr = G_AS_PTR(g);
    g = ufile_reader_get_file_size(fr);
    UASSERT_INT_EQ(G_AS_SIZE(g), 0);
    ufile_reader_destroy(fr);

    UASSERT_NO_ERROR(g = ufile_reader_create("utdata/100", 10));
    fr = G_AS_PTR(g);
    g = ufile_reader_get_file_size(fr);
    UASSERT_INT_EQ(G_AS_SIZE(g), 100);
    ufile_reader_destroy(fr);

    // Buffer grows
    UASSERT_NO_ERROR(g = ufile_reader_create("utdata/100", 1));
    fr = G_AS_PTR(g);
    UASSERT_NO_ERROR(g = ufile_reader_read(fr, 5, NULL));
    m = G_AS_MEMCHUNK(g);
    data = umemchunk_as_str(m);
    UASSERT_STR_EQ(data, "3132333435");
    ufree(data);

    UASSERT_INT_EQ(ufile_reader_get_buffer_size(fr), 5);
    ufile_reader_destroy(fr);
}

void test_ufile_writer(size_t buffer_size)
{
    ugeneric_t g;
    ugeneric_t gr = ufile_reader_create("ttt", buffer_size);
    UASSERT_NO_ERROR(gr);
    ugeneric_t gw = ufile_writer_create("ttt2");
    UASSERT_NO_ERROR(gw);

    ufile_reader_t *fr = G_AS_PTR(gr);
    ufile_writer_t *fw = G_AS_PTR(gw);
    while (ufile_reader_has_next(fr))
    {
        g = ufile_reader_read(fr, buffer_size, NULL);
        UASSERT_NO_ERROR(g);
        g = ufile_writer_write(fw, G_AS_MEMCHUNK(g));
        UASSERT_NO_ERROR(g);
    }

    ufile_reader_destroy(fr);
    ufile_writer_destroy(fw);
}

void test_ufile_api(void)
{
    ugeneric_t g;

    g = ufile_open("", "r");
    UASSERT(G_IS_ERROR(g));
 //   ugeneric_error_print(g);
    ugeneric_error_destroy(g);

    g = ufile_open("/root", "r");
    UASSERT(G_IS_ERROR(g));
//    ugeneric_error_print(g);
    ugeneric_error_destroy(g);

    UASSERT_NO_ERROR(g = ufile_get_size("utdata/empty"));
    UASSERT_SIZE_EQ(G_AS_SIZE(g), 0);
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    test_ufile_api();
    test_ufile_reader();
    //test_ufile_writer(atoi(argv[1]));

    return 0;
}
