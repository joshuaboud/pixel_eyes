#include "print_buf.h"
#include "../src/defines.h"

#include <stdio.h>
#include <time.h>

void ws2812_setleds (struct cRGB  *ledarray, uint16_t number_of_leds) {
    printf("\e[1;1H\e[2J");
    for (int i = 0; i < FRAME_H; i++) {
        for (int j = 0; j < FRAME_W; j++) {
            int buff_ind = i * FRAME_W + j;
            struct cRGB pixel = ledarray[buff_ind];
            printf("%2d#%02x%02x%02x,", buff_ind, pixel.r, pixel.b, pixel.g);
        }
        putchar(' ');
        for (int j = 0; j < FRAME_W; j++) {
            int buff_ind = i * FRAME_W + j + FRAME_PIXELS;
            struct cRGB pixel = ledarray[buff_ind];
            printf("%2d#%02x%02x%02x,", buff_ind, pixel.r, pixel.b, pixel.g);
        }
        putchar('\n');
    }
    putchar ('\n');
    usleep(66666);
}