#pragma once

#include "Light_WS2812/ws2812_config.h"
#include "Light_WS2812/light_ws2812.h"
#include "frame.h"

const Frame frames[] = {
    {
        .info = { .last_frame = 0, .type = COPY, .duration = 31 },
        .pixels = {
            { 0b000000, 0b111111, 0b111111, 0b111111, 0b000000 },
            { 0b111111, 0b111111, 0b000011, 0b111111, 0b111111 },
            { 0b111111, 0b000011, 0b000000, 0b000011, 0b111111 },
            { 0b111111, 0b000011, 0b000000, 0b000011, 0b111111 },
            { 0b111111, 0b111111, 0b000011, 0b111111, 0b111111 },
            { 0b000000, 0b111111, 0b111111, 0b111111, 0b000000 },
        }
    },
    {
        .info = { .last_frame = 1, .type = COPY, .duration = 4 },
        .pixels = {
            { 0b000000, 0b000000, 0b000000, 0b000000, 0b000000 },
            { 0b000000, 0b000000, 0b000000, 0b000000, 0b000000 },
            { 0b000000, 0b000000, 0b000000, 0b000000, 0b000000 },
            { 0b000000, 0b111111, 0b111111, 0b111111, 0b000000 },
            { 0b111111, 0b000000, 0b000000, 0b000000, 0b111111 },
            { 0b000000, 0b000000, 0b000000, 0b000000, 0b000000 },
        }
    },
};

const Frame *frame_lut[] = {
    &frames[0],
    &frames[0],
    &frames[0],
    &frames[0],
    &frames[0],
    &frames[0],
    &frames[0],
    &frames[0],
    &frames[0],
};