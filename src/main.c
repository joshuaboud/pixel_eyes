/*    main.c
 *
 *    Copyright (C) 2021 Joshua Boudreau <joshuaboud@hotmail.com>
 *
 *    This file is part of pixel_eyes.
 *
 *    pixel_eyes is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    pixel_eyes is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with pixel_eyes.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifdef SIMULATE
#	include "../simulate/print_buf.h"
#	include "../simulate/input.h"
#	include <stdio.h>
#else
#	include "Light_WS2812/ws2812_config.h"
#	include "Light_WS2812/light_ws2812.h"
#	include <avr/io.h>
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

	// disable JTAG to use PORTF as IO
	MCUCR |= 1 << JTD;
	MCUCR |= 1 << JTD; // twice within 4 clock cycles

    DDRB |= (1 << ws2812_pin); // set output

    SHIFT_DDR = (1 << SHIFT_LATCH) | (1 << SHIFT_CLOCK);
    SHIFT_PORT |= (1 << SHIFT_LATCH) | (1 << SHIFT_DATA); // keep /latch high, enable pullup
    SHIFT_PORT &= ~(1 << SHIFT_CLOCK); // start clock low
#endif
}

void draw_frame(const Frame **fptr, const Frame *next_emote, struct cRGB *const buff_ptr) {
    const uint8_t left_inc = 1;
    int8_t right_inc = 1;
    struct cRGB *left_out;
    struct cRGB *right_out;
    static uint8_t duration_counter = 0;
	static const Frame *last_next_emote = NULL;
	if (!last_next_emote)
		last_next_emote = next_emote;
	else if (last_next_emote != next_emote) {
		*fptr = next_emote;
		last_next_emote = next_emote;
	}

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
    _delay_us(1);
    SHIFT_PORT |= (1 << SHIFT_LATCH);
    _delay_us(1);

    // start shifting
    uint8_t i, bit;
    for (i = 0; i < 8; i++) {
        bit = (SHIFT_PIN & (1 << SHIFT_DATA)) >> SHIFT_DATA;
        result <<= 1;
		result |= bit;
        SHIFT_PORT |= (1 << SHIFT_CLOCK);
        _delay_us(1);
        SHIFT_PORT &= ~(1 << SHIFT_CLOCK);
        _delay_us(1);
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
            *next_frame = frame_lut[BLINK];
            break;
        case 1 << WINK:
            *next_frame = frame_lut[WINK];
            break;
        case 1 << ANGRY:
            *next_frame = frame_lut[ANGRY];
            break;
        case 1 << SAD:
            *next_frame = frame_lut[SAD];
            break;
        case 1 << LOVE:
            *next_frame = frame_lut[LOVE];
            break;
        case 1 << RAINBOW:
            *next_frame = frame_lut[RAINBOW];
            break;
        case 1 << EYEROLL:
            *next_frame = frame_lut[EYEROLL];
            break;
        case 1 << CASH:
            *next_frame = frame_lut[CASH];
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
        check_input(&next_emote);
        // memset(buffer, 0, sizeof(struct cRGB) * BUFF_SZ);
        // uint8_t press = shift_byte();
		// uint8_t i;
        // for (i = 0; i < 8; i++) {
        //     uint8_t test = (1 << i);
        //     uint8_t res = press & test;
        //     if ((res) != 0) {
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
