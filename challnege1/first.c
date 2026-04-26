#include <stdio.h>
#include <string.h>

static const unsigned char TARGET[] = {
    0x75,0xE6,0xF1,0x38,0xE6,0x7A,0x54,0x12
};
#define FLAG_LEN (sizeof TARGET)

int main(void) {
    unsigned char buf[64] = {0};
    printf("Enter the flag body: ");
    scanf("%32s", buf);
    for (size_t i = 0; i < FLAG_LEN; i++)
        buf[i] = ((buf[i] * 31 + 17) ^ 0xAA) & 0xFF;
    if (memcmp(buf, TARGET, FLAG_LEN) == 0)
        printf("Eclipse{%.*s}\n", (int)FLAG_LEN, buf);
    else
        puts("Wrong.");
    return 0;
}