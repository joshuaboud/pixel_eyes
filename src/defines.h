/*    defines.h
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

#include "frame.h"

/**
 * @brief Total number of LEDs on glasses
 * 
 */
#define BUFF_SZ (FRAME_W * FRAME_H * 2)

enum Emotes {BLINK, WINK, ANGRY, SAD, LOVE, EYEROLL, RAINBOW, OFF, Emotes_length};

#define SHIFT_PORT        PORTF
#define SHIFT_DDR         DDRF
#define SHIFT_PIN         PINF
#define SHIFT_CLOCK       0
#define SHIFT_LATCH       1
#define SHIFT_DATA        4
