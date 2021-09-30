#ifdef SIMULATE
#include "../simulate/print_buf.h"
#include "../simulate/input.h"
#include <stdio.h>
#else
#include "Light_WS2812/ws2812_config.h"
#include "Light_WS2812/light_ws2812.h"
#include <avr/io.h>
#endif
#include "defines.h"
#include "frame.h"
#include "frames.h"

#include <stddef.h>
#include <string.h>

void setup(void) {
    DDRB = 1<<7;
}

void draw_frame(const Frame **fptr, const Frame *next_emote, struct cRGB *const buff_ptr) {
    const uint8_t left_inc = 1;
    int8_t right_inc = 1;
    struct cRGB *left_out;
    struct cRGB *right_out;
    static uint8_t duration_counter = 0;

    FrameInfo info = (*fptr)->info;

    if (duration_counter == 0) { // draw frame once per duration
        // set up pointers
        left_out = &buff_ptr[0];
        enum FrameType type = info.type;
        switch (type) {
            case COPY:
                // advance to start of second eye
                right_out = left_out + FRAME_PIXELS;
                break;
            case MIRROR:
                // advance to end of first line of second eye
                right_out = left_out + FRAME_PIXELS + FRAME_W - 1;
                right_inc = -1;
                break;
            case LEFT:
                right_out = NULL;
                break;
            case RIGHT:
                right_out = left_out + FRAME_PIXELS;
                left_out = NULL;
                break;
        }

        // draw to buffer
        for (uint8_t i = 0; i < FRAME_H; i++) {
            for (uint8_t j = 0; j < FRAME_W; j++) {
                struct cRGB pixel = RGB6_2_cRGB((*fptr)->pixels[i][j]);
                if (left_out) {
                    *left_out = pixel;
                    left_out += left_inc;
                }
                if (right_out) {
                    *right_out = pixel;
                    right_out += right_inc;
                }
            }
            if (type == MIRROR)
                right_out += FRAME_W * 2; // jump two frame widths
        }
    }

    // check if ready for next frame
    if ((duration_counter = (duration_counter + 1) % info.duration) == 0) {
        if (info.last_frame) {
            // if last frame of emote, move to next or loop back depending if changed
            *fptr = next_emote;
        } else {
            (*fptr)++;
        }
    }
}

void display_frame(struct cRGB *buff) {
    ws2812_setleds(buff, BUFF_SZ);
}

void check_input(const Frame **next_frame) {
#ifdef SIMULATE
    simulate_input(next_frame);
#else
    for (uint8_t scan = 0; scan < INPUT_MATRIX_ROWS; scan++) {
        SCAN_PORT = ~(1 << scan); // pull down row
        for (uint8_t test = 0; test < INPUT_MATRIX_COLS; test++) {
            if ((INPUT_PORT & (1 << test)) == 0) { // if tested pin is low, must be pressed
                *next_frame = frame_lut[(scan + 1) * (test + 1)];
                return;
            }
        }
    }
#endif
}

void state_machine() {
    const Frame *fptr = NULL;
    const Frame *next_emote = NULL;
    struct cRGB buffer[BUFF_SZ];
    memset(buffer, 0, sizeof(struct cRGB) * BUFF_SZ);

    // set fptr to first frame of default emote
    fptr = &frames[0];
    next_emote = fptr;

    while (1) {
        draw_frame(&fptr, next_emote, buffer);
        display_frame(buffer);
        //check_input(&next_emote);
    }
}

void test() {
    struct cRGB buff[] = {{255,255,255}, {255,255,255}, {255,255,255}, {255,255,255}, {255,255,255}, {255,255,255}, {255,255,255}, {255,255,255}};
    ws2812_setleds(buff, 8);
    while(1) {}
}

int main(void) {
    setup();
    test();
    state_machine();
    return 0;
}
