#ifdef SIMULATE

#else
#include "Light_WS2812/ws2812_config.h"
#include "Light_WS2812/light_ws2812.h"
#include <avr/io.h>
#endif
#include "defines.h"
#include "frame.h"
#include "frames.h"

#include <stddef.h>

void setup(void) {
    // set up ports
}

void draw_frame(const Frame *fptr, const Frame *next_emote, struct cRGB *const buff_ptr) {
    const uint8_t left_inc = 1;
    uint8_t right_inc = 1;
    struct cRGB *left_out;
    struct cRGB *right_out;
    static uint8_t duration_counter = 0;

    if (duration_counter == 0) { // draw frame once per duration
        // set up pointers
        left_out = buff_ptr;
        enum FrameType type = fptr->info.type;
        switch (type) {
            case COPY:
                right_out = left_out + FRAME_W - 1;
                break;
            case MIRROR:
                right_out = left_out + FRAME_W * 2 - 1;
                right_inc = -1;
                break;
            case LEFT:
                right_out = NULL;
                break;
            case RIGHT:
                right_out = left_out + FRAME_W - 1;
                left_out = NULL;
                break;
        }

        // draw to buffer
        for (uint8_t i = 0; i < FRAME_H; i++) {
            for (uint8_t j = 0; j < FRAME_W; j++) {
                struct cRGB pixel = RGB6_2_cRGB(fptr->pixels[i][j]);
                if (left_out) *left_out = pixel;
                if (right_out) *right_out = pixel;
                left_out += left_inc;
                right_out += right_inc;
            }
            // reset pointers for next line
            switch (type) {
                case COPY:
                    left_out += FRAME_W; // skip current right
                    right_out += FRAME_W; // skip next left
                    break;
                case MIRROR:
                    left_out += FRAME_W;
                    right_out += FRAME_W * 2 - 1;
                    break;
                case LEFT:
                    left_out += FRAME_W; // skip current right
                    break;
                case RIGHT:
                    right_out += FRAME_W; // skip next left
                    break;
            }
        }
    }

    // check if ready for next frame
    if ((duration_counter = (duration_counter + 1) % fptr->info.duration) == 0) {
        if (fptr->info.last_frame) {
            // if last frame of emote, move to next or loop back depending if changed
            fptr = next_emote;
        } else {
            fptr++;
        }
    }
}

void display_frame(struct cRGB *buff) {
    ws2812_setleds(buff, BUFF_SZ);
}

void check_input(const Frame *next_frame) {
    for (uint8_t scan = 0; scan < INPUT_MATRIX_ROWS; scan++) {
        PORTB = ~(1 << scan); // pull down row
        for (uint8_t test = 0; test < INPUT_MATRIX_COLS; test++) {
            if ((PINC & (1 << test)) == 0) { // if tested pin is low, must be pressed
                next_frame = frame_lut[(scan + 1) * (test + 1)];
                return;
            }
        }
    }
}

void state_machine() {
    const Frame *fptr = NULL;
    const Frame *next_emote = NULL;
    struct cRGB buffer[BUFF_SZ];

    // set fptr to first frame of default emote
    fptr = &frames[0];
    next_emote = fptr;

    while(1) {
        draw_frame(fptr, next_emote, buffer);
        display_frame(buffer);
        check_input(next_emote);
    }
}

int main(void) {
    setup();
    state_machine();
    return 0;
}
