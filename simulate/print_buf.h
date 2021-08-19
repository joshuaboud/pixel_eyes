#pragma once

#include <stdint.h>

struct cRGB { uint8_t g; uint8_t r; uint8_t b; };

void ws2812_setleds (struct cRGB  *ledarray, uint16_t number_of_leds);
