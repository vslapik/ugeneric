#include <stdbool.h>
#include "ut_utils.h"
#include "bitmap.h"
#include "mem.h"

void test_bitmap()
{
    bitmap_t *b;
    char *str;

    b = bitmap_create(1);
    bitmap_set_bit(b, 0);
    str = bitmap_as_str(b); ASSERT_STR_EQ(str, "1"); ufree(str);
    bitmap_clear_bit(b, 0);
    str = bitmap_as_str(b); ASSERT_STR_EQ(str, "0"); ufree(str);
    bitmap_destroy(b);

    b = bitmap_create(11);
    bitmap_set_bit(b, 1);
    bitmap_set_bit(b, 3);
    bitmap_set_bit(b, 5);
    bitmap_set_bit(b, 7);
    bitmap_clear_bit(b, 0);
    bitmap_clear_bit(b, 2);
    bitmap_clear_bit(b, 4);
    bitmap_clear_bit(b, 6);

    str = bitmap_as_str(b); ASSERT_STR_EQ(str, "01010101000"); ufree(str);
    bitmap_destroy(b);

    b = bitmap_create(25);
    bitmap_set_bit(b, 0);
    bitmap_set_bit(b, 8);
    bitmap_set_bit(b, 16);
    bitmap_set_bit(b, 24);
    str = bitmap_as_str(b);
    ASSERT_STR_EQ(str, "1000000010000000100000001");
    ufree(str);

    bitmap_flip_all(b);
    str = bitmap_as_str(b);
    ASSERT_STR_EQ(str, "0111111101111111011111110");
    ufree(str);

    bitmap_flip_range(b, 0, 0);
    str = bitmap_as_str(b);
    ASSERT_STR_EQ(str, "1111111101111111011111110");
    ufree(str);

    bitmap_flip_range(b, 24, 24);
    str = bitmap_as_str(b);
    ASSERT_STR_EQ(str, "1111111101111111011111111");
    ufree(str);

    bitmap_flip_range(b, 9, 15);
    str = bitmap_as_str(b);
    ASSERT_STR_EQ(str, "1111111100000000011111111");
    ufree(str);

    bitmap_flip_range(b, 12, 12);
    str = bitmap_as_str(b);
    ASSERT_STR_EQ(str, "1111111100001000011111111");
    ufree(str);

    bitmap_flip_range(b, 3, 4);
    str = bitmap_as_str(b);
    ASSERT_STR_EQ(str, "1110011100001000011111111");
    ufree(str);

    bitmap_flip_range(b, 7, 8);
    str = bitmap_as_str(b);
    ASSERT_STR_EQ(str, "1110011010001000011111111");
    ufree(str);

    bitmap_destroy(b);
}

int main(void)
{
    test_bitmap();
}
