#pragma once

#ifdef SIMULATE

#include "../src/defines.h"
#include "../src/frame.h"
#include <stdint.h>

extern uint8_t SCAN_PORT;
extern const uint8_t INPUT_PORT;

extern int keypress;

void *poll_key(void *arg);
void setup_input();
void end_input();
void simulate_input(const Frame **next_emote);

#endif