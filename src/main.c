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
#include <util/delay.h>

uint8_t shift_byte(void);

void setup(void) {
#ifndef SIMULATE
	CLKPR = 1<<CLKPCE; // enable changing clock prescaler
	CLKPR = 0x1; // DIV2 resulting in 8MHz F_CPU

    DDRB |= (1<<DDB7);

    SHIFT_DDR = (1 << SHIFT_LATCH) | (1 << SHIFT_CLOCK);
    SHIFT_PORT |= (1 << SHIFT_LATCH) | (1 << SHIFT_DATA); // keep /latch high, enable pullup
    SHIFT_PORT &= ~(1 << SHIFT_CLOCK); // start clock low and disable pullup
    shift_byte();
#endif
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

uint8_t shift_byte(void) {
    uint8_t result = 0;

    // latch
    SHIFT_PORT &= ~(1 << SHIFT_LATCH);
    _delay_us(5);
    SHIFT_PORT |= (1 << SHIFT_LATCH);
    _delay_us(5);

    // start shifting
    uint8_t i, test;
    for (i = 8; i--; i) {
        SHIFT_PORT |= (1 << SHIFT_CLOCK);
        _delay_us(5);
        result = result << 1;
        test = SHIFT_PIN & (1 << SHIFT_DATA);
        if (test != 0)
            result |= 1;
        SHIFT_PORT &= ~(1 << SHIFT_CLOCK);
        _delay_us(5);
    }

    return result;
}

void check_input(const Frame **next_frame) {
#ifdef SIMULATE
    simulate_input(next_frame);
#else
    uint8_t buttons_pressed = shift_byte();
    switch (buttons_pressed) {
        case 1 << BLINK:
            next_frame = frame_lut[BLINK];
            break;
        case 1 << WINK:
            next_frame = frame_lut[WINK];
            break;
        case 1 << ANGRY:
            next_frame = frame_lut[ANGRY];
            break;
        case 1 << SAD:
            next_frame = frame_lut[SAD];
            break;
        case 1 << LOVE:
            next_frame = frame_lut[LOVE];
            break;
        case 1 << RAINBOW:
            next_frame = frame_lut[RAINBOW];
            break;
        case 1 << EYEROLL:
            next_frame = frame_lut[WINK];
            break;
        case 1 << CASH:
            next_frame = frame_lut[CASH];
            break;
        default:
            break;
    }
#endif
}

void loop() {
    const Frame *fptr = NULL;
    const Frame *next_emote = NULL;
    struct cRGB buffer[BUFF_SZ];
    memset(buffer, 0, sizeof(struct cRGB) * BUFF_SZ);

    // set fptr to first frame of default emote
    fptr = &frames[0];
    next_emote = fptr;

    while (1) {
        //check_input(&next_emote);
        // memset(buffer, 0, sizeof(struct cRGB) * BUFF_SZ);
        // uint8_t press = shift_byte();
        // struct cRGB pressed = {20,20,20};
        // struct cRGB not_pressed = {0,0,0};
        // for (uint8_t i = 0; i < 8; i++) {
        //     uint8_t test = (1 << i);
        //     uint8_t res = press & test;
        //     if ((press & test) != 0) {
        //         buffer[i].r = 20;
        //         buffer[i].g = 20;
        //         buffer[i].b = 20;
        //     }
        // }
        draw_frame(&fptr, next_emote, buffer);
        display_frame(buffer);
		_delay_us(66666);
    }
}

int main(void) {
    setup();
    loop();
    return 0;
}
