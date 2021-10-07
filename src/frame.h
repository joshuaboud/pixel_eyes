/*    frame.h
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

#pragma once

#ifdef SIMULATE
#include <stdint.h>
#else
#include <avr/io.h>
#endif

#define FRAME_W 5
#define FRAME_H 6
#define FRAME_PIXELS (FRAME_W * FRAME_H)

/**
 * @brief Convert RGB6 struct to cRGB for the Light_WS2812 library
 * 
 */
#define RGB6_2_cRGB(x) {x.g << 3, x.b << 3, x.r << 3}

/**
 * @brief How to display the frame across each eye
 * 
 */
enum FrameType { COPY, MIRROR, LEFT, RIGHT };

/**
 * @brief 6 bit color
 * 
 */
typedef union {
    uint8_t all;
    struct {
        uint8_t r : 2;
        uint8_t g : 2;
        uint8_t b : 2;
    };
} RGB6;

/**
 * @brief Metadata for each frame
 * 
 */
typedef struct {
    uint8_t last_frame : 1; ///< Loop back to beginning of animation if set
    enum FrameType type : 2; ///< How to display this frame
    uint8_t duration : 5; ///< How long to display this frame
} FrameInfo;

/**
 * @brief Frame struct with metadata and 2D array of pixels
 * 
 */
typedef struct {
    FrameInfo info;
    RGB6 pixels[FRAME_H][FRAME_W];
} Frame;
