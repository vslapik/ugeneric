#include <stdbool.h>
#include "ut_utils.h"
#include "bitmap.h"
#include "mem.h"

#define CHECK_BITMAP(bmp, len, as_string) do {              \
    char *__s = ubitmap_as_str(bmp, len);                   \
    UASSERT_STR_EQ(__s, as_string);                          \
    ufree(__s);                                             \
} while (0)                                                 \

void test_bitmap()
{
    char *str;

    uint8_t b1 = 0;
    ubitmap_set_bit(&b1, 0);
    str = ubitmap_as_str(&b1, 1);
    UASSERT_STR_EQ(str, "1");
    ufree(str);

    ubitmap_clear_bit(&b1, 0);
    str = ubitmap_as_str(&b1, 1);
    UASSERT_STR_EQ(str, "0"); ufree(str);

    uint16_t b11 = 0;
    ubitmap_set_bit(&b11, 1);
    ubitmap_set_bit(&b11, 3);
    ubitmap_set_bit(&b11, 5);
    ubitmap_set_bit(&b11, 7);
    ubitmap_clear_bit(&b11, 0);
    ubitmap_clear_bit(&b11, 2);
    ubitmap_clear_bit(&b11, 4);
    ubitmap_clear_bit(&b11, 6);

    CHECK_BITMAP(&b11, 11, "01010101000");

    uint32_t b25 = 0;
    ubitmap_set_bit(&b25, 0);
    ubitmap_set_bit(&b25, 8);
    ubitmap_set_bit(&b25, 16);
    ubitmap_set_bit(&b25, 24);
    CHECK_BITMAP(&b25, 25, "1000000010000000100000001");

    ubitmap_flip_all(&b25, 25);
    CHECK_BITMAP(&b25, 25, "0111111101111111011111110");

    ubitmap_flip_range(&b25, 0, 1);
    CHECK_BITMAP(&b25, 25, "1111111101111111011111110");

    ubitmap_flip_range(&b25, 24, 25);
    CHECK_BITMAP(&b25, 25, "1111111101111111011111111");

    ubitmap_flip_range(&b25, 9, 16);
    CHECK_BITMAP(&b25, 25, "1111111100000000011111111");

    ubitmap_flip_range(&b25, 12, 13);
    CHECK_BITMAP(&b25, 25, "1111111100001000011111111");

    ubitmap_flip_range(&b25, 3, 5);
    CHECK_BITMAP(&b25, 25, "1110011100001000011111111");

    ubitmap_flip_range(&b25, 7, 9);
    CHECK_BITMAP(&b25, 25, "1110011010001000011111111");

    ubitmap_flip_bit(&b25, 12);
    CHECK_BITMAP(&b25, 25, "1110011010000000011111111");

    uint8_t *b33 = ubitmap_allocate(33);
    CHECK_BITMAP(b33, 33, "000000000000000000000000000000000");
    ubitmap_flip_bit(b33, 30);
    UASSERT_INT_EQ(ubitmap_get_bit(b33, 29), 0);
    UASSERT_INT_EQ(ubitmap_get_bit(b33, 30), 1);
    UASSERT_INT_EQ(ubitmap_get_bit(b33, 31), 0);
    CHECK_BITMAP(b33, 33, "000000000000000000000000000000100");

    ufree(b33);
}

int main(void)
{
    test_bitmap();
}
