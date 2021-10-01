#pragma once

#ifdef SIMULATE
#include <stdint.h>
#else
#include <avr/io.h>
#endif

#define FRAME_W 5
#define FRAME_H 6
#define FRAME_PIXELS (FRAME_W * FRAME_H)

#define RGB6_2_cRGB(x) {x.g << 3, x.b << 3, x.r << 3}

enum FrameType { COPY, MIRROR, LEFT, RIGHT };

typedef union {
    uint8_t all;
    struct {
        uint8_t r : 2;
        uint8_t g : 2;
        uint8_t b : 2;
    };
} RGB6;

typedef struct {
    uint8_t last_frame : 1;
    enum FrameType type : 2;
    uint8_t duration : 5;
} FrameInfo;

typedef struct {
    FrameInfo info;
    RGB6 pixels[FRAME_H][FRAME_W];
} Frame;
