#include "file_utils.h"
#include "mem.h"
#include "ut_utils.h"

size_t execute_read(const char *path, size_t buffer_size)
{
    size_t read_count = 0;
    generic_t g = file_reader_create(path, buffer_size);
    ASSERT_NO_ERROR(g);

    file_reader_t *fr = G_AS_PTR(g);
    size_t offset = 0;
    while (file_reader_has_next(fr))
    {
        g = file_reader_get_position(fr); ASSERT_NO_ERROR(g);
        ASSERT_INT_EQ(offset, G_AS_SIZE(g));
        g = file_reader_read(fr, buffer_size, NULL); ASSERT_NO_ERROR(g);
        offset += buffer_size;

//        generic_print(g);
        read_count++;
    }

    file_reader_destroy(fr);

    return read_count;
}

void test_file_reader(void)
{
    generic_t g;
    memchunk_t m;
    char *data;
    file_reader_t *fr;

    size_t buffer_size = 3;
    ASSERT_NO_ERROR(g = file_reader_create("utdata/100", buffer_size));
    fr = G_AS_PTR(g);

    ASSERT_NO_ERROR(g = file_reader_read(fr, buffer_size, NULL));
    m = G_AS_MEMCHUNK(g);
    data = memchunk_as_str(m);
    ASSERT_STR_EQ(data, "313233");
    ufree(data);

    ASSERT_NO_ERROR(g = file_reader_read(fr, buffer_size, NULL));
    m = G_AS_MEMCHUNK(g);
    data = memchunk_as_str(m);
    ASSERT_STR_EQ(data, "343536");
    ufree(data);

    ASSERT_NO_ERROR(g = file_reader_read(fr, buffer_size, NULL));
    m = G_AS_MEMCHUNK(g);
    data = memchunk_as_str(m);
    ASSERT_STR_EQ(data, "373839");
    ufree(data);

    ASSERT_NO_ERROR(file_reader_reset(fr));

    ASSERT_NO_ERROR(g = file_reader_read(fr, buffer_size, NULL));
    m = G_AS_MEMCHUNK(g);
    data = memchunk_as_str(m);
    ASSERT_STR_EQ(data, "313233");
    ufree(data);

    file_reader_destroy(fr);

    //
    ASSERT_INT_EQ(execute_read("utdata/100", 1), 100);
    ASSERT_INT_EQ(execute_read("utdata/100", 11), 10);
    ASSERT_INT_EQ(execute_read("utdata/100", 1), 100);
    ASSERT_INT_EQ(execute_read("utdata/empty", 1), 0);
    ASSERT_INT_EQ(execute_read("utdata/empty", 1001), 0);

    ASSERT_NO_ERROR(g = file_reader_create("utdata/empty", 10));
    fr = G_AS_PTR(g);
    g = file_reader_get_file_size(fr);
    ASSERT_INT_EQ(G_AS_SIZE(g), 0);
    file_reader_destroy(fr);

    ASSERT_NO_ERROR(g = file_reader_create("utdata/100", 10));
    fr = G_AS_PTR(g);
    g = file_reader_get_file_size(fr);
    ASSERT_INT_EQ(G_AS_SIZE(g), 100);
    file_reader_destroy(fr);

    // Buffer grows
    ASSERT_NO_ERROR(g = file_reader_create("utdata/100", 1));
    fr = G_AS_PTR(g);
    ASSERT_NO_ERROR(g = file_reader_read(fr, 5, NULL));
    m = G_AS_MEMCHUNK(g);
    data = memchunk_as_str(m);
    ASSERT_STR_EQ(data, "3132333435");
    ufree(data);

    ASSERT_INT_EQ(file_reader_get_buffer_size(fr), 5);
    file_reader_destroy(fr);
}

void test_file_writer(size_t buffer_size)
{
    generic_t g;
    generic_t gr = file_reader_create("ttt", buffer_size);
    ASSERT_NO_ERROR(gr);
    generic_t gw = file_writer_create("ttt2");
    ASSERT_NO_ERROR(gw);

    file_reader_t *fr = G_AS_PTR(gr);
    file_writer_t *fw = G_AS_PTR(gw);
    while (file_reader_has_next(fr))
    {
        g = file_reader_read(fr, buffer_size, NULL);
        ASSERT_NO_ERROR(g);
        g = file_writer_write(fw, G_AS_MEMCHUNK(g));
        ASSERT_NO_ERROR(g);
    }

    file_reader_destroy(fr);
    file_writer_destroy(fw);
}

void test_file_api(void)
{
    generic_t g;

    g = file_open("", "r");
    ASSERT(G_IS_ERROR(g));
 //   generic_error_print(g);
    generic_error_destroy(g);

    g = file_open("/root", "r");
    ASSERT(G_IS_ERROR(g));
//    generic_error_print(g);
    generic_error_destroy(g);

    ASSERT_NO_ERROR(g = file_get_size("utdata/empty"));
    ASSERT_SIZE_EQ(G_AS_SIZE(g), 0);
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    test_file_api();
    test_file_reader();
    //test_file_writer(atoi(argv[1]));

    return 0;
}
