#include "Light_WS2812/ws2812_config.h"
#include "Light_WS2812/light_ws2812.h"
#include "defines.h"
#include "frames.h"

volatile uint8_t emote;
volatile uint8_t frame;

volatile struct cRGB buffer[NUM_LEDS];

typedef struct {
    uint8_t display : 2;
    uint8_t time_scale : 2;
    uint8_t scale_first_frame_only : 1;

} EmoteInfoBitfield;

const EmoteInfoBitfield emote_info[Emotes_length] = {
    {.display = COPY, .time_scale = 3, .scale_first_frame_only = 1}, // BLINK
    {.display = SINGLE_EYE, .time_scale = 2, .scale_first_frame_only = 0}, // WINK
    {.display = COPY, .time_scale = 0, .scale_first_frame_only = 0}, // HAPPY
    {.display = MIRROR, .time_scale = 0, .scale_first_frame_only = 0}, // ANGRY
    {.display = MIRROR, .time_scale = 2, .scale_first_frame_only = 0}, // SAD
    {.display = MIRROR, .time_scale = 2, .scale_first_frame_only = 0}, // LOVE
    {.display = MIRROR, .time_scale = 0, .scale_first_frame_only = 0}, // RAINBOW
    {.display = COPY, .time_scale = 2, .scale_first_frame_only = 0}, // EYEROLL
    {.display = COPY, .time_scale = 1, .scale_first_frame_only = 0}, // CASH
};

void setup() {
    emote = BLINK;
    frame = 0;
}

void display_frame() {
    uint8_t inc = 1;
    uint8_t in_off = 0;
    uint8_t out_off = 0;
    EmoteInfoBitfield info = emote_info[emote];
    frame %= FRAME_COUNT;

    // not done
    const struct  cRGB *in = &frames[frame];
    volatile struct cRGB *out = &buffer[0];
    out[out_off] = in[in_off % LED_COLS];
    out_off++;
    in_off += inc;

    frame++;
}

void check_input() {
    for (uint8_t scan = 0; scan < INPUT_MATRIX_ROWS; scan++) {
        PORTB = ~(1 << scan); // pull down row
        for (uint8_t test = 0; test < INPUT_MATRIX_COLS; test++) {
            if ((PINC & (1 << test)) == 0) { // if tested pin is low, must be pressed
                emote = (test + 1) * (scan + 1);
                frame = 0;
                return;
            }
        }
    }
}

void state_machine() {
    while(1) {
        display_frame();
        check_input();
    }
}

int main(void) {
    struct cRGB buffer[NUM_LEDS];
    setup();
    return 0;
}
