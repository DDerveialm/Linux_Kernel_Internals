#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

uint16_t func(uint16_t N) {
    /* change all right side bits to 1 */
    N |= N >> 1;
    N |= N >> 2;
    N |= N >> 4;
    N |= N >> 8;

    return (N >> 1) + 1;
}

int main() {
    printf("%u\n", func(UINT16_MAX));
    return 0;
}
