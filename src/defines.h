#pragma once

#include "frame.h"

#define BUFF_SZ (FRAME_W * FRAME_H * 2)

enum Emotes {BLINK, WINK, ANGRY, SAD, LOVE, RAINBOW, EYEROLL, CASH, Emotes_length};

#define INPUT_MATRIX_ROWS 3
#define INPUT_MATRIX_COLS 3

#define SHIFT_PORT        PORTF
#define SHIFT_DDR         DDRF
#define SHIFT_PIN         PINF
#define SHIFT_CLOCK       0
#define SHIFT_LATCH       1
#define SHIFT_DATA        4